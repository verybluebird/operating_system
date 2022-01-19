#pragma comment (lib,"Ws2_32.lib") 

#define _CRT_SECURE_NO_WARNINGS // ClangRunTime functions permission 

#include <stdio.h> 
#include <WinSock.h> 
#include <Windows.h> 
#include <iostream>

void MAC_print(FILE* out, char* MAC)
{
	int i;
	for (i = 0; i < 5; i++)
		fprintf(out, "%02X:", (unsigned char)MAC[i]);
	fprintf(out, "%02X\n", (unsigned char)MAC[i]);
}

void IP_print(FILE* out, char* IP)
{
	int i;
	for (i = 0; i < 3; i++)
		fprintf(out, "%d.", (unsigned char)IP[i]);
	fprintf(out, "%d\n", (unsigned char)IP[i]);
}

bool empty_mac(char* MAC)
{
	for (int i = 0; i < 5; i++)
		if (MAC[i] != 0)
			return false;
	return true;
}

void main()
{
	char* bytes, file_name[256];
	int file_size = 0, ARP = 0, IPv4 = 0, DIX = 0, SNAP = 0, RAW = 0, LLC = 0;
	printf("Enter the name of binary file: ");
	scanf("%s", &file_name);
	FILE* in = fopen(file_name, "rb");
	if (in == NULL)
		printf("No available file found\n");
	else
	{
		FILE* out = fopen("out.txt", "w");
		fseek(in, 0, SEEK_END);//устанавливаем в конец
		file_size = ftell(in);
		fseek(in, 0, SEEK_SET);//устанавливаем в начало

		bytes = new char[file_size];
		fread(bytes, file_size, 1, in);
		fclose(in);
		char* curr_byte = bytes;
		int frame_number = 1;
		while (curr_byte < bytes + file_size)
		{
			fprintf(out, "\nFrame number: %d\n", frame_number);
			fprintf(out, "Destination MAC: ");
			while (empty_mac(curr_byte))
				curr_byte += 6;
			MAC_print(out, curr_byte);
			fprintf(out, "Source MAC: ");
			MAC_print(out, curr_byte + 6);
			USHORT LT = ntohs(*(USHORT*)(curr_byte + 12));
			fprintf(out, "Length/Type field: %d\n", LT);
			if (LT == 0x0800) //ipv4 2048 
			{
				fprintf(out, "Type: IPv4\n");
				fprintf(out, "Source IP: ");
				IP_print(out, curr_byte + 26); // смещение 6(MAC получателя) + 6(MAC отправи-теля) + 2(поле LT) + 12(смещение IP-адреса отправителя в IPv4-пакете) = 26 байтов 
				fprintf(out, "Destination IP: ");
				IP_print(out, curr_byte + 30); // адрес назначения следует непосредственно за 4-байтовым адресом отправителя в IPv4-пакете 

				LT = ntohs(*(USHORT*)(curr_byte + 16)) + 14; // полный размер пакета указывается во 2 и 3-м байтах IPv4-пакета, в кадре это будут 16 и 17-е байты, 
				// поэтому сначала считывается двухбайтовое число по адресу currbyte + 16, а за-тем прибавляется длина параметров пакета 14 (2 MAC-адреса и поле LT) 
				fprintf(out, "Size: %d\n", LT);
				curr_byte += LT;
				IPv4++;
				frame_number++;
				std::cout << frame_number << " " << curr_byte << '\n';
			}
			else
			{
				if (LT == 0x0806) //arp 2054 
				{
					fprintf(out, "Frame type: ARP\n");
					curr_byte += 28 + 14; // длина стандартного ARP-пакета является фиксированной и равна 28 байтам, 14 байтов занимают параметры кадра 
					ARP++;
					frame_number++;
				}
				else
				{
					if (LT > 0x05DC) //dix 1500 
					{
						fprintf(out, "Frame Ethernet DIX (Ethernet II)\n");
						DIX++; // если не удалось идентифицировать кадр DIX II, будем считать поле LT длиной типа, 
						// т. к. идентификация всех типов - сложная задача, требующая подключения базы знаний с информацией о длине кадра (байтах кадра или пакета, где она хранится) каждого типа 
					}
					else
					{
						USHORT F = ntohs(*(USHORT*)(curr_byte + 14));
						if (F == 0xFFFF)//raw 65535 
						{
							fprintf(out, "Frame Raw 802.3 (Novell 802.3)\n");
							RAW++;
						}
						else
							if (F == 0xAAAA) //snap 43690 
							{
								fprintf(out, "Frame Ethernet SNAP\n");
								SNAP++;
							}
							else //llc 
							{
								fprintf(out, "Frame 802.3/LLC (Frame 802.3/802.2 or Novell 802.2)\n");
								LLC++;
							}
					}
					curr_byte += LT + 14; // смещение в конец текущего кадра (длина данны= + 14 байт (параметры кадра - 2 MAC-адреса и поле LT)) 
					frame_number++;
				}
			}
		}
		fprintf(out, "\nEntire frame number: %d\n", frame_number - 1);
		fprintf(out, "IPv4: %d\n", IPv4);
		fprintf(out, "ARP: %d\n", ARP);
		fprintf(out, "DIX: %d\n", DIX);
		fprintf(out, "RAW: %d\n", RAW);
		fprintf(out, "SNAP: %d\n", SNAP);
		fprintf(out, "LLC: %d\n", LLC);
		fclose(out);
	}
}