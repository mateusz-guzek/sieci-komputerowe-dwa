


# projekt

6. Radio internetowe. Serwer ma kolejkę plików dźwiękowych, które kolejno strumieniuje do wszystkich podłączonych klientów.
Wymagane: klienci mogą wgrywać na serwer nowe pliki, widzą bieżący stan kolejki i mogą wykonywać dowolne operacje na niej, mogą też zlecić przeskoczenie do następnego pliku z kolejki. Wymagane GUI i integracja API ze Spotify Jam. 



### How to run - Server
```bash
cd serwer
./run.sh
```

### How to run - Server - Expert Level
```bash
cd serwer
cmake -B build
cmake --build build
./out/sk-serwer
```

### How to connect to the server locally
```bash
netcat localhost 5000
```