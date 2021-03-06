
/////////////////////////////////////////////////////////////////////////////// 
// 
// Copyright (c) 2016 Herb Sutter. All rights reserved. 
// 
// This code is licensed under the MIT License (MIT). 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE. 
// 
///////////////////////////////////////////////////////////////////////////////


#ifndef GCPP_BITFLAGS
#define GCPP_BITFLAGS

#include "util.h"

#include <vector>
#include <algorithm>

namespace gcpp {

	//----------------------------------------------------------------------------
	//
	//	vector<bool> operations aren't always optimized, so here's a custom class.
	//
	//----------------------------------------------------------------------------

	class bitflags {
		static constexpr byte ALL_TRUE = byte(0xFF), ALL_FALSE = byte(0x00);

		const int size;
		std::vector<byte> bits;

	public:
		bitflags(int bits, bool value)
			: size{ bits }
			, bits(1 + size / sizeof(byte), value ? ALL_TRUE : ALL_FALSE)
		{ 
			Expects(bits > 0 && "#bits must be positive");
		}

		//	Get flag value at position
		//
		bool get(int at) const {
			Expects(0 <= at && at < size && "bitflags get() out of range");
			return (bits[at / sizeof(byte)] & byte(1 << (at % sizeof(byte)))) > byte(0);
		}

		//	Set flag value at position
		//
		void set(int at, bool value) {
			Expects(0 <= at && at < size && "bitflags set() out of range");
			if (value) {
				bits[at / sizeof(byte)] |= byte(1 << (at % sizeof(byte)));
			}
			else {
				bits[at / sizeof(byte)] &= byte(0xff ^ (1 << (at % sizeof(byte))));
			}
		}

		//	Set all flags to value
		//
		void set_all(bool value) {
			std::fill(begin(bits), end(bits), value ? ALL_TRUE : ALL_FALSE);
		}

		//	Set all flags in positions [from,to) to value
		//
		void set(int from, int to, bool value) {
			// first set the remaining bits in the partial byte this range begins within
			while (from < to && from % sizeof(byte) != 0) {
				set(from++, value);
			}

			// then set whole bytes (makes a significant performance difference)
			while (from < to && to - from >= sizeof(byte)) {
				bits[from / sizeof(byte)] = value ? byte(0xFF) : byte(0x00);
				from += sizeof(byte);
			}

			// then set the remaining bits in the partial byte this range ends within
			while (from < to) {
				set(from++, value);
			}
		}
	};

	//	Future: Just set(from,to) is a performance improvement over vector<bool>,
	//	but also add find_next_false etc. functions to eliminate some loops

}

#endif
