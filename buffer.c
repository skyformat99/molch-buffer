/* Molch, an implementation of the axolotl ratchet based on libsodium
 *  Copyright (C) 2015  Max Bruckner (FSMaxB)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <sodium.h>
#include <string.h>

#include "buffer.h"

/*
 * Initialize a molch buffer with a given length.
 *
 * This is normally not called directly but via
 * the molch_buffer_create macro.
 */
buffer_t* buffer_init(
		buffer_t * const buffer,
		const size_t buffer_length,
		const size_t content_length) {
	buffer->buffer_length = buffer_length;
	buffer->content_length = (content_length > buffer_length)
		? buffer_length
		: content_length;
	buffer->readonly = false;
	buffer->content = (unsigned char*) buffer + sizeof(buffer_t);
	return buffer;
}

/*
 * Create hexadecimal string from a buffer.
 *
 * The output buffer has to be at least twice
 * as large as the input data plus one.
 */
int buffer_to_hex(buffer_t * const hex, const buffer_t * const data) {
	//check size
	if (hex->buffer_length < (data->content_length * 2 + 1)) {
		return -6;
	}

	if (sodium_bin2hex((char*)hex->content, hex->buffer_length, data->content, data->content_length) == NULL) {
		sodium_memzero(hex->content, hex->buffer_length);
		hex->content_length = 0;
		return -10;
	}

	hex->content_length = 2 * data->content_length + 1;
	return 0;
}

/*
 * Clear a buffer.
 *
 * Overwrites the buffer with zeroes and
 * resets the content size.
 */
void buffer_clear(buffer_t *buffer) {
	sodium_memzero(buffer->content, buffer->buffer_length);
	buffer->content_length = 0;
}

/*
 * Concatenate a buffer to the first.
 *
 * Return 0 on success.
 */
int buffer_concat(
		buffer_t * const destination,
		buffer_t * const source) {
	if (destination->readonly) {
		return -5;
	}

	if (destination->buffer_length < (destination->content_length + source->content_length)) {
		//buffer isn't long enough
		return -6;
	}

	memcpy(destination->content + destination->content_length, source->content, source->content_length);
	destination->content_length += source->content_length;
	return 0;
}
