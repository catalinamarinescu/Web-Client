[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
# Web-Client
Tema3 - Protocoale de Comunicatii
Marinescu Maria-Catalina 324CA

--------------------------------------------------------------------------------

Scheletul temei este reprezentat de codul de la laboratorul 9, iar in plus in 
fisierul "request.c" am adaugat functia de "compute_delete_request". Pentru
parsarea obiectelor json am folosit biblioteca parson sugerata in enuntul temei.

--------------------------------------------------------------------------------

Tema in sine este reprezentata de fisierul client.c, in care am citit fiecare 
comanda primita de la input si am tratat-o in mod corespunzator in cadrul unei 
instructiuni if else.
Pentru fiecare comanda am respectat acelasi format si anume:
Am deschis socket-ul, am creat obiectul json cu datele necesare, am creat 
mesajul, l-am trimis la server apoi am primit raspunsul si la final am inchis 
socket-ul.

Pentru fiecare comanda insa au fost lucruri particulare, pe langa cele 
mentionate anterior:

-pentru register am citit cu ajutorul functiei "read_input_reg" username-ul si 
parola si in cazul in care 
unul dintre acestea contine un spatiu se va printa un mesaj de eroare iar contul
nu va fi inregistrat.

-pentru login am procedat asemanator ca la register, diferenta fiind ca atunci 
cand user-ul se autentifica se va extrage cookie-ul corespunzator intors de 
server.

-pentru enter_library se realizeaza un GET request unde server-ul va intoarce
un token ce va determina daca se pot realiza anumite comenzi ce tin de 
biblioteca user-ului conectat.

-pentru get_books se va face de asemenea un GET request cu token-ul obtinut de 
la enter_library pentru a se demonstra accesul la biblioteca. Server-ul va 
intoarce fie mesaje de eroare fie lista cartilor aflate in biblioteca.  

-pentru get_book se va citi id-ul de a stdin si vom contrui url-ul corespunzator
cu strcat. Server-ul va intoarce fie mesaje de eroare fie datele despre carte.

-pentru add_book se vor citi datele ce sunt string-uri de la stdin cu ajutorul 
functiei "read_input", iar page_count va fi citit separat, fiind int. In cazul 
in care page_count este string sau unul din celelalte campuri este gol se va 
afisa un mesaj de eroare.

-pentru delete_book se va proceda asemanator cu ger_book, diferenta fiind ca se 
va face un DELETE request la server.

-pentru logout se va face un GET request cu cookie-ul iar in cazul in care nu 
este niciun user autentificat se va intoarce o eroare. Altfe-ul userul se va 
deloga.

-pentru exit se va iesi din program.  
