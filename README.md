# System rezerwacji hotelowych

## Opis systemu

System rezerwacji hotelowych to konsolowa aplikacja umożliwiająca zarządzanie pokojami, rezerwacjami oraz płatnościami w hotelu.  
Pozwala na efektywne zarządzanie dostępnością pokoi, obsługę klientów i rezerwacji, a także monitorowanie statusów płatności i meldunków.  

System obsługuje różne role użytkowników:  
- Administrator - zarządza pokojami i generuje raporty.  
- Recepcjonista - obsługuje rezerwacje, meldunki oraz płatności.  
- Klient - przegląda pokoje, tworzy rezerwacje i dokonuje płatności.  

---

## Funkcjonalności

- **Zarządzanie pokojami:** dodawanie nowych pokoi, edycja opisu i ceny, dezaktywacja pokoi, zmiana statusu (wolny, zajęty, niedostępny).  
- **Rezerwacje:** tworzenie rezerwacji z weryfikacją dostępności, potwierdzanie i anulowanie, meldowanie i wymeldowywanie gości.  
- **Płatności:** tworzenie i powiązywanie płatności z rezerwacjami, obsługa różnych metod płatności (gotówka, karta, przelew), zarządzanie statusami płatności (w trakcie, opłacone, anulowane).  
- **Role i uprawnienia:** różne poziomy dostępu i funkcjonalności w zależności od roli użytkownika.  
- **Raporty:** generowanie raportów o aktualnym stanie pokoi i rezerwacji (dostępne dla administratora).  

---

## Architektura systemu

### Modele danych

- **Pokoj:** identyfikator, numer, pojemność, cena za dobę, status pokoju, opis i aktywność.  
- **Klient:** dane osobowe klienta (id, pesel, imię, nazwisko, email).  
- **Rezerwacja:** powiązanie klienta z pokojem na określony termin, status rezerwacji oraz powiązana płatność.  
- **Platnosc:** informacje o płatności, w tym kwota, status i metoda płatności.  

### Serwisy zarządzające

- **RoomService:** zarządza pokojami - dodaje, edytuje, usuwa i kontroluje statusy pokoi.  
- **ReservationService:** zarządza rezerwacjami - tworzy, potwierdza, anuluje, meldowanie i wymeldowywanie, sprawdza dostępność.  
- **PaymentService:** zarządza płatnościami - tworzy, anuluje i oznacza płatności jako opłacone.  

### Menu i interfejs użytkownika

- Program posiada konsolowe menu dopasowane do roli użytkownika, umożliwiające łatwą nawigację i obsługę funkcji systemu.  
- Role: Administrator, Recepcjonista, Klient - każde z własnym zestawem dostępnych opcji.  

---

## Wymagania systemowe

- Kompilator zgodny z C++11 lub nowszym.  
- Środowisko do uruchomienia aplikacji konsolowej.  