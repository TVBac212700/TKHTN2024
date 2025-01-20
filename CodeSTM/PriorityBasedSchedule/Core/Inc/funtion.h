/*
 * funtion.h
 *
 *  Created on: Jan 13, 2025
 *      Author: BacTV
 */

#ifndef SRC_FUNTION_H_
#define SRC_FUNTION_H_
#define MAX_DATA_SIZE 256

typedef struct {
    uint8_t Start;     // Ký tự Start (dấu ':' đầu tiên)
    uint8_t Addr;      // Địa chỉ (Addr)
    uint8_t Code;      // Mã lệnh (Code)
    uint8_t Data[MAX_DATA_SIZE];  // Mảng dữ liệu (Data)
    uint16_t Crc;      // CRC
    uint16_t Stop;     // Stop (carriage return + newline)
    uint16_t Len;      // Chiều dài dữ liệu (Data length)
} DataStruct;
void parse_data(const char *input, DataStruct *data) {
    // Xác nhận ký tự Start
    char temp[10] = {0};
//    data->Start = input[0];  // Dấu ':'

    memset(data->Data,0,MAX_DATA_SIZE);
    strncpy(temp, input, 2);
    data->Start = (uint8_t)strtol(temp, NULL, 16);

    // Chuyển đổi Addr
    strncpy(temp, input + 2, 2);
    data->Addr = (uint8_t)strtol(temp, NULL, 16);

    // Chuyển đổi Code
    strncpy(temp, input + 4, 2);
    data->Code = (uint8_t)strtol(temp, NULL, 16);

    // Tách phần Data
    size_t dataIndex = 0;
    size_t inputIndex = 6; // Vị trí bắt đầu dữ liệu (sau Addr và Code)
    while (input[inputIndex] != '\r' && input[inputIndex] != '\n' && input[inputIndex] != '\0') {
        strncpy(temp, input + inputIndex, 2);
        data->Data[dataIndex++] = (uint8_t)strtol(temp, NULL, 16);
        inputIndex += 2;
    }
    data->Len = dataIndex - 4; // Lưu chiều dài của Data

    // Chuyển đổi CRC
    strncpy(temp, input + inputIndex - 4, 4);
    temp[4] = '\0';
    data->Crc = (uint16_t)strtol(temp, NULL, 16);

    // Chuyển đổi Stop
    data->Stop = ((uint16_t)input[inputIndex] << 8) | input[inputIndex + 1];
}

uint16_t calculate_crc(uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;  // Giá trị bắt đầu CRC-16 (0xFFFF)

    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];  // XOR byte hiện tại với CRC
        for (uint8_t bit = 0; bit < 8; bit++) {  // Lặp qua từng bit
            if (crc & 0x0001) {  // Nếu bit thấp nhất là 1
                crc = (crc >> 1) ^ 0xA001;  // Dịch sang phải và XOR với 0xA001
            } else {
                crc >>= 1;  // Chỉ dịch CRC sang phải nếu bit thấp nhất là 0
            }
        }
    }
    int16_t L = crc&0xFF;
    int16_t H = (crc>>8)&0xFF;
    return (L<<8)|H;  // Trả về CRC tính toán
}

uint16_t check_crc(DataStruct *data) {
    // Tạo mảng chứa dữ liệu cần tính CRC
    uint8_t temp_data[MAX_DATA_SIZE + 3];  // Start, Addr, Code, Data (tối đa)

    // Sao chép các thành phần dữ liệu vào mảng tạm
    size_t index = 0;
    temp_data[index++] = data->Start;  // Start
    temp_data[index++] = data->Addr;   // Addr
    temp_data[index++] = data->Code;   // Code

    // Sao chép dữ liệu vào mảng
    memcpy(&temp_data[index], data->Data, data->Len);

    // Tính toán CRC cho dữ liệu
    uint16_t calculated_crc = calculate_crc(temp_data, index + data->Len);

    // So sánh CRC tính toán với CRC đã cho
    if (calculated_crc == data->Crc) {
        return calculated_crc;  // CRC hợp lệ
    } else {
        return calculated_crc;  // CRC không hợp lệ
    }
}

int stringToHex(const char *input, char *output) {
    int len = strlen(input); // Độ dài chuỗi đầu vào

    for (int i = 0; i < len; i++) {
        // Chuyển mỗi ký tự thành mã hex và lưu vào chuỗi kết quả
        sprintf(&output[i * 2], "%02X", (unsigned char)input[i]);
    }
    output[len * 2] = '\0'; // Kết thúc chuỗi
    return len * 2;
}


void data_struct_to_string(DataStruct* data, char* output) {
    // Bắt đầu chuỗi với ký tự Start
    sprintf(output, "%02X%02X%02X", data->Start, data->Addr, data->Code);

    // Thêm Addr và Code vào chuỗi
//    sprintf(output + strlen(output), "");

    // Thêm Data vào chuỗi
    for (int i = 0; i < data->Len; i++) {
        sprintf(output + strlen(output), "%02X", data->Data[i]);
    }

    // Thêm CRC vào chuỗi
    sprintf(output + strlen(output), "%04X", data->Crc);

    // Thêm Stop (\r\n) vào chuỗi
    sprintf(output + strlen(output), "%04X",data->Stop);
}

void reverseString(char* str) {
    int length = strlen(str); // Độ dài chuỗi
    int start = 0;           // Chỉ số bắt đầu
    int end = length - 1;    // Chỉ số kết thúc
    char temp;

    // Đảo ngược chuỗi bằng cách đổi chỗ
    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start++;
        end--;
    }
}
uint16_t ArrConv[] = {MPR_NUM_0, MPR_NUM_1, MPR_NUM_2, MPR_NUM_3, MPR_NUM_4, MPR_NUM_5, MPR_NUM_6, MPR_NUM_7, MPR_NUM_8, MPR_NUM_9, MPR_NUM_D, MPR_NUM_O};
char ArrChar[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'};

void convertTo12BitBinaryString(uint16_t input, char *output) {
    for (int i = 0; i < 12; i++) {
        if (input & ArrConv[i]) {  // Kiểm tra nếu bit tương ứng được bật
            output[i] = ArrChar[i];
        } else {
            output[i] = '-';       // Nếu không, thay bằng '-'
        }
    }
    output[12] = '\0'; // Kết thúc chuỗi
}
#include <stdio.h>

unsigned int find_bit_position(unsigned short num) {
    unsigned int count = 0;

    // Đếm số lượng bit bằng 1 trong chuỗi 12 bit
    unsigned int temp = num;
    for (int i = 0; i < 12; i++) {
        if (temp & 1) {
            count++;
        }
        temp >>= 1;
    }

    // Nếu có từ 2 bit 1 trở lên, trả về 0xFF
    if (count >= 2 || count == 0) {
        return 0xFF;
    }

    // Nếu chỉ có 1 bit 1, tìm và trả về vị trí của bit 1
    unsigned int position = 0;
    while ((num & 1) == 0 && position < 12) {
        num >>= 1;
        position++;
    }

    return position;
}

char convert_to_char(uint16_t num) {
    unsigned int count = 0;
    int position = -1;

    // Kiểm tra từng bit trong số 12 bit
    for (int i = 0; i < 12; i++) {
        if (num & ArrConv[i]) { // Nếu bit này là 1
            count++;
            position = i;  // Lưu lại vị trí của bit
        }
    }

    // Nếu có nhiều hơn 1 bit bằng 1, trả về '\0'
    if (count != 1) {
        return '\0';
    }

    // Nếu chỉ có 1 bit 1, trả về ký tự tương ứng
    return ArrChar[position];
}

char* gen_data_send_rs485(uint8_t code, char data[])
{
	DataStruct result;
	memset((void*)&result,0,sizeof(result));
	result.Start = 0x3A;
	result.Addr = MY_ADDRESS;
	result.Code = code;
	result.Len = strlen(data);
	memcpy((void*)result.Data, (void*)data, strlen(data)+1);
	result.Crc = check_crc(&result);
	result.Stop = 0x0D0A;
	data_struct_to_string(&result,data);
	return data;
}


#endif /* SRC_FUNTION_H_ */
