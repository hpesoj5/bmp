# Implementation Notes

Just some reflections from implementing the various parts of the binary message parser.

## General

`read_bytes<std::integral T>`: if length of field is less than `sizeof(T)`, I realised that the bits needed to be shifted appropriately, and this means that I must also ensure that the size of `std::byte` on the system is 8 bits.

`read_bytes<std::string>`: the symbol field and other alpha fields are right padded with spaces, not zero bytes. Either way, they have to be trimmed in the resulting string. I trimmed the padding bytes in a string view instead of a string to save costs.

`read_bytes<T>`: the bounds check `if (offset + length > buf.size())` may not work for large offset and length due to unsigned overflow. Change to `if (offset > buf.size() || length > buf.size() - offset)`
