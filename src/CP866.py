def UTF8_to_hex_CP866(string):
    encoded_bytes = string.encode('cp866')
    hex_values = [f"0x{byte:02x}" for byte in encoded_bytes]
    formatted_hex = "{" + ", ".join(hex_values) + "}"
    return formatted_hex


while True:
    user_input = input("Input string: ")
    user_input = user_input.replace('і', 'i').replace('І', 'I')
    result = UTF8_to_hex_CP866(user_input)
    print(result)
