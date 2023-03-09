#include <bits/stdc++.h> 
#include <arpa/inet.h> 
using namespace std;

int p, q, e, d, n, phi;
int PUs[2], PRs[2];
int PUc[2]; int sock;

void createServer(int port)
{
	int sersock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr = {AF_INET, htons(port), INADDR_ANY};

	bind(sersock, (struct sockaddr *) &addr, sizeof(addr)); 
	cout << "\nServer Online. Waiting for client	" << endl;
	listen(sersock, 5);
	sock = accept(sersock, NULL, NULL); 
	cout << "Connection Established." << endl;
}

int gcd(int a, int b)
{
	return b==0 ? a : gcd(b, a%b);
}

void genKey()
{
	cout << "\nEnter two prime numbers (>100): "; cin >> p >> q; n = p * q ;
	phi = (p-1) * (q-1); srand(time(NULL));
	do
	{ 
		e = rand()%(phi-2)+2; 
	} while(gcd(e,phi) != 1);

	for(d=1; d<phi; d++)
	{
		if((d*e)%phi == 1) break;
	}

	PUs[0] = e; PUs[1] = n;
	PRs[0] = d; PRs[1] = n;

	cout << "\nPublic key , PUs = {" << e << ", " << n << "}" << endl; cout << "Private key, PRs = {" << d << ", " << n << "}" << endl;
}

void shareKey()
{
	send(sock, &PUs, sizeof(PUs), 0); recv(sock, &PUc, sizeof(PUc), 0);
	cout << "Sent Server's Public key to client." << endl;
	cout << "\nPublic key received from client : {" << PUc[0] << ", " << PUc[1] <<"}" << endl;
}




int encrypt(int M, int P[2])
{
	int C=1;
	for(int i=1; i<=P[0]; i++)
{
	C = (C * M) % P[1];
}
	return C;



}
int decrypt(int C, int P[2])
{
	return encrypt(C,P);
}

int main()
{
	int port; cout<<"\nEnter port : "; 
	cin>>port; 
	srand(time(NULL));

	createServer(port); 
	genKey(); 
	shareKey();

	int ID; 
	cout<<"\nEnter Server's ID number (<100): "; 
	cin>>ID; 
	int N1 = rand()%100;
	cout << "Nonce generated, N1 = " << N1 << endl;

	int msg = N1*100 + ID; // append ID to nonce 
	int cipher = encrypt(msg, PUc);
	send(sock, &cipher, sizeof(cipher), 0);
	cout << "Sent encrypted (N1||ID) to client : " << cipher << endl;

	recv(sock, &cipher, sizeof(cipher), 0);
	cout << "\nReceived encrypted (N1||N2) from client : " << cipher << endl; 
	msg = decrypt(cipher, PRs);

	int N1c = msg/100; int N2 = msg%100;
	cout << "Decrypted Server's Nonce, N1 = " << N1c << endl; 
	cout << "Decrypted Client's Nonce, N2 = " << N2 << endl; 
	if(N1 != N1c) {cout << "\nNonce didn't match!\n"; exit(-1);} 
	else {cout << "------ Client Authenticated	" << endl;}

	cipher = encrypt(N2, PUc);
	send(sock, &cipher, sizeof(cipher), 0);
	cout << "\nSent encrypted (N2) to client : " << cipher << endl;

	int k;
	cout << "\nEnter secret key (integer) to send : "; 
	cin >> k; 



	cipher = encrypt(encrypt(k,PRs), PUc);
	send(sock, &cipher, sizeof(cipher), 0);
	cout << "Sent encrypted secret key to client : " << cipher	<< endl << endl;
}

