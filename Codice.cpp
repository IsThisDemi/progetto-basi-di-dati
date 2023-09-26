#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "./dependencies/include/libpq-fe.h"

#define PG_HOST "127.0.0.1" 
#define PG_USER "postgres" // nome utente
#define PG_DB "postgres" // nome del database
#define PG_PASS "" // password
#define PG_PORT 5432

using namespace std;

void checkResults ( PGresult * res , const PGconn * conn ) {
   if ( PQresultStatus ( res ) != PGRES_TUPLES_OK ) {
      cout << " Risultati inconsistenti " << PQerrorMessage ( conn );
      PQclear ( res );
      exit (1) ;
   }
}

void print(PGresult * res)
	{
		if(res==0) return;
		
		int tuple = PQntuples(res);
		int campi = PQnfields(res);
		
		cout << "\nRisultato: \n\n";
		
		for (int i=0; i<campi; ++i) {
			cout << left << setw(40) << PQfname(res, i);
		}
		cout << "\n\n"; 
		for(int i=0; i<tuple; ++i) {
			for (int n=0; n<campi; ++n) {
				cout << left << setw(40) << PQgetvalue(res, i, n);
			}
			cout << '\n';
		}
		cout<<'\n'<< endl;
        PQclear ( res );
	}

int selectQuery(){
	int s = 0;
	while (s==0)
	{
		cout<< "Inserire il numero della query desiderata (-1 per uscire): ";
		if (!(cin >> s)) {
			cin.clear();
			cin.ignore(1000,'\n');
			cout << "Input non valido, riprova \n";
		}
	}
	return s;
}

int main ( int argc , char * argv []) {
cout << " Start " << endl ;

char conninfo [250];

sprintf ( conninfo , " user =%s password=%s dbname=%s hostaddr=%s port=%d",
PG_USER , PG_PASS , PG_DB , PG_HOST , PG_PORT ) ;

PGconn * conn = PQconnectdb ( conninfo );

if ( PQstatus ( conn ) != CONNECTION_OK ) {
	cout << " Errore di connessione \n" << PQerrorMessage ( conn );
	PQfinish ( conn );
	//exit (1) ;
}


cout << " Connessione avvenuta correttamente " << endl ;


PGresult * res ;

bool end = false;

while(end!=true) {
	switch (selectQuery())
	{
	case -1:
		end = true;
		break;
	case 1:
		//Query 1
		cout << " 1 - Visualizzare la partita IVA delle aziende che hanno incassato piu' di 7500 euro in ordine decrescente" << endl ;
		res = PQexec ( conn , "select Azienda, sum(totale_prodotto) AS totale_guadagni from quantità_prodotti group by Azienda having sum(totale_prodotto) > 7500 order by totale_guadagni desc;") ;
		checkResults ( res , conn );
		print(res);
		break;
	case 2:
		//Query 2
		cout << " 2 - I 5 prodotti piu' venduti in assoluto mostrandone la categoria e l'azienda" << endl ;
		res = PQexec ( conn , " SELECT p.ID, p.nome, nop.Quantità, p.Categoria, a.nome FROM (Prodotto p JOIN num_ord_prod nop ON p.ID = nop.Prodotto) JOIN Azienda a ON p.Azienda = a.Partita_IVA ORDER BY nop.Quantità desc limit 5;") ;
		checkResults ( res , conn );
		print(res);
		break;
	case 3:
		//Query 3
		cout << " 3 - I 25 corrieri che hanno effettuato piu' consegne " << endl ;
		res = PQexec ( conn , " SELECT c.codice_fiscale, c.Nome, c.Cognome, noc.num_ord FROM Corriere c JOIN num_ord_cor noc ON c.codice_fiscale = noc.Corriere ORDER BY noc.num_ord desc limit 25;") ;
		checkResults ( res , conn );
		print(res);
		break;
	case 4:
		// Query 4
		cout << " 4 - Visualizzare la mail dei clienti che hanno comprato almeno un prodotto limited edition" << endl ;
		res = PQexec ( conn , " SELECT u.nome, u.cognome, u.mail, tou.num_ordini_limited FROM Utente u JOIN tot_ordinilim_utente tou ON u.id = tou.id ORDER BY tou.num_ordini_limited DESC;") ;
		checkResults ( res , conn );
		print(res);
		break;
	case 5:
		cout << " 5 - Recensioni positive (> 3) di ordini consegnati dopo le 12:00 del 17 Agosto 2023" << endl ;
		res = PQexec ( conn , " SELECT r.Prodotto, r.Voto, r.Testo, o.DataOraEffettiva FROM Ordine o JOIN Recensione r ON o.ID = r.Ordine WHERE o.DataOraEffettiva > '2023-08-17 12:00:00' and r.Voto > 3;") ;
		checkResults ( res , conn );
		print(res);
		break;
	default:
		cout<<"Numero di input non valido \n";
	}
}
}
