# include <bits/stdc++.h> 
# include <arpa/inet.h> 
using namespace std;
int p, q, e, d, n, phi;
int PUc[2], PRc[2];
int PUs[2]; int sock;

void connectToServer(const char* ip, int port)
{
          sock = socket(AF_INET, SOCK_STREAM, 0);
          struct sockaddr_in addr = {AF_INET, htons(port), inet_addr(ip)};

          if(connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0 )
         {
           cout << "\nRun server program first." << endl; exit(0);
          }
          else
         {
          cout << "\nClient is connected to Server." << endl;
         }
}

int gcd(int a, int b)
{
         return b==0 ? a : gcd(b, a%b);
}

void genKey()
{
         cout << "\nEnter two prime numbers (>100) : "; cin >> p >> q; n = p * q ;
         phi = (p-1) * (q-1);

          srand(time(NULL));
          do{ e = rand()%(phi-2)+2; } 
          while(gcd(e,phi) != 1); 
          for(d=1; d<phi; d++)
         {
          if((d*e)%phi == 1) 
          break;
          }

          PUc[0] = e; PUc[1] = n;
          PRc[0] = d; PRc[1] = n;
          cout << "\nPublic key , PUc = {" << e << ", " << n << "}" << endl; 
          cout << "Private key, PRc = {" << d << ", " << n << "}" << endl;
}

void shareKey()
{
         recv(sock, &PUs, sizeof(PUs), 0); send(sock, &PUc, sizeof(PUc), 0);
         cout << "Public key received from server, PUs = {" << PUs[0] << ", " << PUs[1] << "}" << endl; 
         cout << "\nSent client's Public key to server." << endl;	
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
        char ip[50];
        cout<<"\nEnter server's IP address: "; 
        cin>>ip; 
        int port; 
        cout<<"Enter port : "; 
        cin>>port;

        srand(time(NULL)); 
        connectToServer(ip, port); 
        genKey();
        shareKey();

        int cipher;
        recv(sock, &cipher, sizeof(cipher), 0);
        cout << "\nReceived encrypted (N1||ID) from server : " << cipher << endl; int msg = decrypt(cipher, PRc);
        int N1 = msg/100; int ID = msg%100;
        cout << "Decrypted Server's ID, IDs = " << ID << endl; cout << "Decrypted Server's nonce, N1 = " << N1 << endl;

        int N2 = rand() % 100;
        cout << "\nNonce generated, N2 = " << N2 << endl; msg = N1*100 + N2;
        cipher = encrypt(msg, PUs); send(sock, &cipher, sizeof(cipher), 0);
        cout << "Sent encrypted (N1||N2) to server : " << cipher << endl;

        recv(sock, &cipher, sizeof(cipher), 0);
        cout << "\nReceived encrypted (N2) from server : " << cipher << endl; int N2s = decrypt(cipher, PRc);
        cout << "Decrypted Client's Nonce, N2 = " << N2s << endl; 
        if(N2s != N2) {cout << "\nNonce didn't match!\n"; exit(-1);} 
        else {cout << "----- Server Authenticated	" << endl;}
        int k;
        recv(sock, &cipher, sizeof(cipher), 0);
        cout << "\nReceived cipher from Server : " << cipher << endl; 
        k = decrypt(decrypt(cipher, PRc), PUs);
        cout << "Decrypted Secret Key : " << k << endl << endl;
}

