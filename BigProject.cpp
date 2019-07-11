

#define _CRT_SECURE_NO_DEPRECATE

#include "stdafx.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <string>
using namespace std;

void repleceF(const char* file, int line, const char* buff);
void showFile(char *file);
int mailCount(char* file);
string getTXT(const char * filename);
void getInfo(const char * file);
void getAttachmentFile(const char * encodedFile);
void getContent(const char * file);
int fileCount(const char * file);
int cfileexists(const char * filename);


int main()
{
	struct sockaddr_in addr;
	SOCKET client;
	char buf[1024];
	long ret;
	struct hostent *H;
	char R[1024];
	char MSG[512];
	char input[64];
	char cmdBuf[256];
	char link[256];
	
	

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SSL *ssl;
	SSL_CTX *ctx;


	client = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	H = gethostbyname("pop.gmail.com");
	addr.sin_addr.s_addr = *((unsigned long *)H->h_addr);
	addr.sin_port = htons(995);

	ret = connect(client, (struct sockaddr *) &addr, sizeof(addr));

	SSL_load_error_strings();
	SSL_library_init();

	ctx = SSL_CTX_new(SSLv23_client_method());
	
	ssl = SSL_new(ctx);
	
	SSL_set_fd(ssl, (int)client);
	SSL_connect(ssl);

	if (ret == 0) {

		ret = SSL_read(ssl, R, 128);    // Lay phan hoi tu may chu
	
		for (int i = ret; i< 1023; i++) {
			R[i] = NULL;
		}

		sprintf(buf, "recv from S %d \nS: %s", ret, R);      
		printf("%s\n", buf);                                    // Hien thi phan hoi sau khi thiet lap ket noi


		

		
//---------------------------- Pha xac thuc nguoi dung ------------------------------------------------------
		
		while (1) {
			
			// ------------------------- Nhap vao username ---------------------------------------------------
			
			printf("Ten dang nhap : ");

			scanf("%s%*c", &input);
			sprintf(R, "USER %s\r\n", input);
			ret = SSL_write(ssl, R, strlen(R));                 // gui usename den cho server


			ret = SSL_read(ssl, R, 512);      


			for (int i = ret; i < 1023; i++) {
				R[i] = NULL;
			}

			
           // ----------------------- Nhap vao password -----------------------------------------------------

			printf("Mat Khau : ");
			scanf("%s%*c", &input);
			sprintf(R, "PASS %s\r\n", input);                
			ret = SSL_write(ssl, R, strlen(R));                  // gui pass den server

		

			ret = SSL_read(ssl, R, 512);           // nhan phan hoi 
	
			for (int i = ret-2; i< 1023; i++) {
				R[i] = NULL;
			}



			if (strcmp(R, "+OK Welcome.") == 0) {
				printf("\nDang nhap thanh cong\n\n");
				break;
			}

			else printf("\nSai ten dang nhap hoac mat khau !\n\n");

		}
//------------------------------------- Ket thuc phien xac thuc ------------------------------------------------








			
//  -------------- BAT DAU PHA GIAO DICH ---------------------------------------------------
		
			int input;
			int flag = 0;
			int flag1 = 0;
			
			while (true) {
				printf("---------------------------------------------\n");
				printf("\n<1>. New mail from Gmail.\n<2>. Downloaded mail.\n<3>. Exit.\n");
				scanf("%d%*c", &input);
				if (input == 3) break;
				if (input == 1) {
					
					// Luu ket qua lenh LIST vao file list.txt
					
					strcpy(R, "LIST\r\n");
					ret = SSL_write(ssl, R, strlen(R));
					char buffer[512];
					int nDataLength;

					nDataLength = SSL_read(ssl, buffer, sizeof(buffer));
					for (int i = nDataLength-4; i < sizeof(buffer); i++) {
						buffer[i] = NULL;
					}
					FILE *f = fopen("LIST.txt", "w");
					fprintf(f, buffer);
					fclose(f);
            
		            // ---------------- Xu ly file LIST -------------------------------------


					int input1;
					while (true) {
						printf("---------------------------------------------\n\n");
						showFile("LIST.txt");

						printf("\n<1>.Download.\n<2>.Return.\n");
						scanf("%d%*c", &input1);
						if (input1 == 2) break;
						if (input1 == 1) {
							int input2;
							while (true) {
								printf("---------------------------------------------\n\n");
								showFile("LIST.txt");
								int mailAmout = mailCount("LIST.txt");
								printf("\nEnter mail's number for download: \n");
								scanf("%d%*c", &input2);
								if (input2 <= mailAmout) {
								
									//--------------------------- start of save mail ------------------------------
										

									//---------------------Luu toan bo data vao file tmp ------------------------------
									DWORD dwTimeout = 2000; // 2 seconds
									setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwTimeout, sizeof(DWORD));

									sprintf(R, "RETR %d\r\n", input2);
									ret = SSL_write(ssl, R, strlen(R));
									char buffer[2048];
									int nDataLength;
									while (1) {
										nDataLength = SSL_read(ssl, buffer, sizeof(buffer));
										if (nDataLength <= 0) break;
										for (int i = nDataLength; i < sizeof(buffer); i++) {
											buffer[i] = NULL;
										}
										FILE *f = fopen("tmp.txt", "at");
										fprintf(f, buffer);
										fclose(f);
									}

									// ---------- xoa mail cho chac an -----------
									sprintf(R, "DELE %d\r\n", input2);
									ret = SSL_write(ssl, R, strlen(R));

									// -----Dem so dong trong mylist ----------
									char fileBuf[256];
									int count = 0;
									FILE *f = fopen("mylist.txt", "r");
									while (fgets(fileBuf, sizeof(fileBuf), f))
									{
										count++;
									}
									fclose(f);
									count++;

									// ----------- Tao folder theo stt dua tren so dong cua mylist------
									
									sprintf(cmdBuf,"mkdir %d",count);
									system(cmdBuf);

									// -------------------Xu ly file tmp -------------------
									getInfo("tmp.txt");
									getContent("tmp.txt");
									getAttachmentFile("tmp.txt");									
							// ---- Sau ham code tren, file mail va attachment duoc tao ra (neu co)---------------


									// ------- Doc dong dau cua mail de lay Date ------------
									f = fopen("mail.txt", "r");
									fgets(fileBuf, sizeof(fileBuf), f);
									fclose(f);
									if (fileBuf[strlen(fileBuf) - 1] == '\n')
										fileBuf[strlen(fileBuf) - 1] = 0;

									// --------- Ghi thong tin file vao mylist --------------------
									char listLine[256];
									f = fopen("mylist.txt", "at");
									sprintf(listLine,"%d. %s (unread)\n",count,fileBuf );
									fprintf(f, listLine);
									fclose(f);
									
									// ------- Xu ly file attachment ------------------------------------

									int check = cfileexists("raw.txt");
									if (check == 1) {	
										f = fopen("mail.txt", "r");
										while (fgets(fileBuf, sizeof(fileBuf), f)) {}
										fclose(f);

										sprintf(cmdBuf,"certutil -decode raw.txt %s", fileBuf);
										system(cmdBuf);
										sprintf(cmdBuf, "move %s  %d", fileBuf, count );
										system(cmdBuf);
										system("del raw.txt");
									}


									//----------- Di chuyen  cac file vao folder ----------------
									sprintf(cmdBuf, "move mail.txt %d",count);
									system(cmdBuf);

									printf("\n");
									

									system("del tmp.txt");


									


									//--------------------------- end of save mail ------------------------------



									printf("\nDownload OK. \n");
									int input3;
									while (true) {
										printf("\n<1>. Download another mail.\n<2>. Return.\n");
										scanf("%d%*c", &input3);
										if ( input3 == 1 ) break;
										if ( input3 == 2 ) { flag ++; break; }
										else if (input3 != 1 && input3 != 2 ) printf("\nInvalid option\n");
									}
								}
								if (flag == 1) break;
								else if (input2 > mailAmout) printf("\nCannot find the mail. \n");
							}
						}
						if (flag == 1) break;
						else if (input1 != 1 && input1 != 2) printf("\nInvalid option\n");
					}
				}
				


				if (input == 2) {
					int input1;
					while (true) {
						showFile("mylist.txt");
						printf("\n<1>. Read mail.\n<2>. Return.\n");
						scanf("%d%*c", &input1);
						if (input1 == 2) break;
						if (input1 == 1) {
							int input2;
							while (true) {
								char fileBuf[256];
								int count = 0;
								FILE *f = fopen("mylist.txt", "r");
								while (fgets(fileBuf, sizeof(fileBuf), f))
								{
									count++;
								}
								fclose(f);
								showFile("mylist.txt");
								printf("Enter mail's number for read: \n");
								scanf("%d%*c", &input2);
								if (input2 <= count) {
									char inputstr[8];
									_itoa(input2, inputstr, 10);
									int num = fileCount(inputstr);
									if (num == 2) {
										int input3;
										while (true) {		
											
											sprintf(link, "%d\\mail.txt", input2);
											showFile(link);
											
											f = fopen(link, "r");
											fgets(fileBuf, sizeof(fileBuf), f);
											fclose(f);
											if (fileBuf[strlen(fileBuf) - 1] == '\n')
												fileBuf[strlen(fileBuf) - 1] = 0;
											
											char listLine[256];
											sprintf(listLine, "%d. %s (read)\n", input2, fileBuf);
											repleceF("mylist.txt", input2, listLine); 
											fclose(f);

											printf("\n<1>. Open attachment.\n<2>. Return.\n");
											scanf("%d%*c", &input3);
											if (input3 == 2) { flag1 = 1; break; }
											if (input3 == 1) {
												sprintf(link, "%d\\mail.txt", input2);
												f = fopen(link, "r");
												while (fgets(fileBuf, sizeof(fileBuf), f)) {}
												fclose(f);

												sprintf(cmdBuf, "start %d\\%s",input2,fileBuf);
												system(cmdBuf);
											}
											else if (input3 != 1 && input3 != 2) printf("Invalid option\n");
										}
									}


									if (num == 1) {
										int input3;
										while (true) {						

											sprintf(link, "%d\\mail.txt", input2);
											showFile(link);


											f = fopen(link, "r");
											fgets(fileBuf, sizeof(fileBuf), f);
											fclose(f);
											if (fileBuf[strlen(fileBuf) - 1] == '\n')
												fileBuf[strlen(fileBuf) - 1] = 0;

											char listLine[256];
											sprintf(listLine, "%d. %s (read)\n", input2, fileBuf);
											repleceF("mylist.txt", input2, listLine);
											fclose(f);

											printf("<1>. Return.\n");
											scanf("%d%*c", &input3);

											if (input3 == 1) { flag1 = 1; break; }
											else if (input3 != 1) printf("Invalid option\n");
										}
									}


								}
								if (flag1 == 1) break;
								else if(input2 > count) printf("Cannot find the mail. \n");
							}
						}
						else if (input1 != 1 && input1 != 2) printf("Invalid option\n");
					}
				}
				else if (input != 1 && input != 2 && input != 3) printf("\nInvalid option\n");
			}

//----------------- KET THUC PHA GIAO DICH ------------------------------------------------------


			

			strcpy(R, "QUIT\r\n");
			ret = SSL_write(ssl, R, strlen(R));
			ret = SSL_read(ssl, R, 512);	


			system("PAUSE");

			SSL_shutdown(ssl);
			closesocket(client);
			SSL_free(ssl);


		

	}

	//MessageBox(0, &tmp1, &tmp2, 0);

	

	return 0;
}


void showFile(char *file) {
	char  c;
	FILE*f = fopen(file, "r");
	c = fgetc(f);
	while (c != EOF)
	{

		if (c != '\r')
			printf("%c", c);
		c = fgetc(f);
	}

	fclose(f);
}

int mailCount(char* file) {
	int n = 0;
	char line[256];
	FILE* sync = fopen(file, "r");
	while (fgets(line, 256, sync) != NULL) {
		n++;
	}
	fclose(sync);
	n--;
	return n;
}

string getTXT(const char * filename) {
	ifstream myfile(filename);
	string myString;
	if (myfile.is_open())
	{
		while (!myfile.eof())
		{
			std::string str;
			while (std::getline(myfile, str))
			{
				myString += str;
				myString += "\n";

				//i++;
				//num_characters++;
			}
			//myfile >> myArray[i];
			//i++;
			//num_characters++;
		}
	}
	return myString;
}

void getInfo(const char * file) {
	int arraysize = 100000;
	char myArray[500000];
	string myString;
	int num_characters = 0;
	int i = 0;
	std::ofstream f("mail.txt");


	//ifstream myfile("z.txt");
	myString = getTXT(file);
	string tempStr;


	//get sender email

	tempStr = myString.substr(myString.find("From: ") + strlen("From: "));
	tempStr = tempStr.substr(0, tempStr.find("\r\n"));
	string from;
	from = tempStr.substr(tempStr.find("<") + strlen("<"));
	from = from.substr(0, from.find(">"));
	tempStr.clear();

	//get datetime
	string date;
	tempStr = myString.substr(myString.find("Date: ") + strlen("Date: "));
	date = tempStr.substr(0, tempStr.find("\n"));
	tempStr.clear();

	//get Subject
	string subject;
	tempStr = myString.substr(myString.find("Subject: ") + strlen("Subject: "));
	subject = tempStr.substr(0, tempStr.find("\n"));
	tempStr.clear();

	//get recvr email

	tempStr = myString.substr(myString.find("To: ") + strlen("To: "));
	tempStr = tempStr.substr(0, tempStr.find("\n"));
	string to;
	to = tempStr.substr(tempStr.find("<") + strlen("<"));
	to = to.substr(0, to.find(">"));
	tempStr.clear();

	f << "Date: " << date << "\n";
	f << "From: " << from << "\n";
	f << "To: " << to << "\n";
	f << "Subject: " << subject << "\n";

	f.close();
}

void getContent(const char * file) {
	int arraysize = 100000;
	char myArray[500000];
	string myString;
	int num_characters = 0;
	int i = 0;

	myString = getTXT(file);

	
	string  header1 = "Content-Type: text/plain";
	
	string tempStr;
	tempStr = myString.substr(myString.find("Content-Type: text/plain") + strlen("Content-Type: text/plain"));
	tempStr = tempStr.substr(tempStr.find("\n") + strlen("\n"));
	tempStr = tempStr.substr(0, tempStr.find("--"));
	char tempArr[100];
	strcpy(tempArr, tempStr.c_str());
	tempStr.clear();

	std::ofstream f;

	f.open("mail.txt", std::ofstream::out | std::ofstream::app);
	f << "Content: " << tempArr << "\n";

	if (myString.find("filename=\"") != string::npos)
	{
		string name;
		name = myString.substr(myString.find("filename=\"") + strlen("filename=\""));
		name = name.substr(0, name.find("\""));
		f << "Attachment: \n" << name;
	}

	f.close();
}

void getAttachmentFile(const char * encodedFile) {
	int arraysize = 100000;
	char myArray[500000];
	string myString;
	int num_characters = 0;
	int i = 0;

	myString = getTXT(encodedFile);

	if (myString.find("filename=\"") != string::npos)
	{

		string tempStr = myString;

		tempStr = tempStr.substr(tempStr.find("X-Attachment-Id:") + strlen("X-Attachment-Id:"));

		tempStr = tempStr.substr(tempStr.find("\r\n\r\n") + strlen("\r\n\r\n"));
		tempStr = tempStr.substr(0, tempStr.find("--"));
		char newArr[] = "";

		strcpy(myArray, tempStr.c_str());
	
		std::ofstream file("raw.txt");

		file << myArray;
		file.close();

	}

}


int fileCount(const char * file) {
	int n = 0;
	WIN32_FIND_DATAA data;
	char link[64];
	sprintf(link, "%s\\*.*", file);
	HANDLE hFile = FindFirstFileA(link, &data);
	do {
		n++;
	} while (FindNextFileA(hFile, &data));
	n = n - 2;
	FindClose(hFile);
	return n;
}

int cfileexists(const char * filename) {
	FILE *file;
	if (file = fopen(filename, "r")) {
		fclose(file);
		return 1;
	}
	return 0;
}



void repleceF(const char* file, int line, const char* buff) {
	FILE *fptr1, *fptr2;
	int linectr = 0;
	char str[256];
	char  temp[] = "temp.txt";

	fptr1 = fopen(file, "r");
	if (!fptr1)
	{
		printf("Unable to open the input file!!\n");
	}
	fptr2 = fopen(temp, "w");
	if (!fptr2)
	{
		printf("Unable to open a temporary file to write!!\n");
		fclose(fptr1);
	}



	while (!feof(fptr1))
	{
		strcpy(str, "\0");
		fgets(str, 256, fptr1);
		if (!feof(fptr1))
		{
			linectr++;
			if (linectr != line)
			{
				fprintf(fptr2, "%s", str);
			}
			else
			{
				fprintf(fptr2, "%s", buff);
			}
		}
	}
	fclose(fptr1);
	fclose(fptr2);
	remove(file);
	rename(temp, file);
}