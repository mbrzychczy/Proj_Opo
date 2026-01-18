#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// ====== Enum statusu rezerwacji ======
enum class BookingStatus {
    Utworzona,
    Potwierdzona,
    Anulowana,
    Wykonana,
    Zameldowany,
    Oczekujaca
};

std::string to_string(BookingStatus s) {
    switch (s) {
    case BookingStatus::Utworzona:   return "utworzona";
    case BookingStatus::Potwierdzona:return "zatwierdzona";
    case BookingStatus::Anulowana:   return "anulowana";
    case BookingStatus::Wykonana:    return "wykonana";
    case BookingStatus::Zameldowany: return "zameldowany";
    case BookingStatus::Oczekujaca:  return "oczekujaca";
    }
    return "unknown";
}

// ====== Modele ======
struct Pokoj {
    int id;
    int numer;
    int pojemnosc;
    double cenaZaDobe;
    bool aktywny = true;
};

struct Klient {
    int id;
    std::string pesel;
    std::string imie;
    std::string nazwisko;
};

struct Rezerwacja {
    int id;
    int klientId;
    int pokojId;
    int odDnia;   // na start int
    int doDnia;
    BookingStatus status = BookingStatus::Utworzona;
};

// ====== RoomService ======
class RoomService {
    std::vector<Pokoj> pokoje;
    int nextId = 1;

public:
    int dodajPokoj(int numer, int pojemnosc, double cenaZaDobe) {
        Pokoj p;
        p.id = nextId++;
        p.numer = numer;
        p.pojemnosc = pojemnosc;
        p.cenaZaDobe = cenaZaDobe;
        p.aktywny = true;

        pokoje.push_back(p);
        return p.id;
    }

    bool usunPokoj(int id) {
        for (size_t i = 0; i < pokoje.size(); i++) {
            if (pokoje[i].id == id && pokoje[i].aktywny) {
                pokoje[i].aktywny = false;
                return true;
            }
        }
        return false;
    }

    const std::vector<Pokoj>& lista() const { return pokoje; }

    // ZAMIENNIK std::optional: zwracamy wskaünik albo nullptr
    const Pokoj* znajdz(int id) const {
        for (size_t i = 0; i < pokoje.size(); i++) {
            if (pokoje[i].id == id) return &pokoje[i];
        }
        return nullptr;
    }
};

// ====== ReservationService ======
class ReservationService {
    std::vector<Rezerwacja> rezerwacje;
    int nextId = 1;

public:
    int utworzRezerwacje(int klientId, int pokojId, int odDnia, int doDnia) {
        Rezerwacja r;
        r.id = nextId++;
        r.klientId = klientId;
        r.pokojId = pokojId;
        r.odDnia = odDnia;
        r.doDnia = doDnia;
        r.status = BookingStatus::Utworzona;

        rezerwacje.push_back(r);
        return r.id;
    }

    bool potwierdz(int id) {
        for (size_t i = 0; i < rezerwacje.size(); i++) {
            if (rezerwacje[i].id == id && rezerwacje[i].status == BookingStatus::Utworzona) {
                rezerwacje[i].status = BookingStatus::Potwierdzona;
                return true;
            }
        }
        return false;
    }

    bool anuluj(int id) {
        for (size_t i = 0; i < rezerwacje.size(); i++) {
            if (rezerwacje[i].id == id && rezerwacje[i].status != BookingStatus::Anulowana) {
                rezerwacje[i].status = BookingStatus::Anulowana;
                return true;
            }
        }
        return false;
    }

    const std::vector<Rezerwacja>& lista() const { return rezerwacje; }
};

// ====== UI helpers ======
int wczytajInt(const std::string& prompt) {
    std::cout << prompt;
    int x;
    while (!(std::cin >> x)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Podaj liczbe: ";
    }
    return x;
}

double wczytajDouble(const std::string& prompt) {
    std::cout << prompt;
    double x;
    while (!(std::cin >> x)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Podaj liczbe (double): ";
    }
    return x;
}

void menuPokoje(RoomService& roomService) {
    while (true) {
        std::cout << "\n== POKOJE ==\n"
            << "1) Dodaj pokoj\n"
            << "2) Usun (dezaktywuj) pokoj\n"
            << "3) Lista pokoi\n"
            << "0) Wroc\n";
        int op = wczytajInt("> ");

        if (op == 0) return;

        if (op == 1) {
            int numer = wczytajInt("Numer pokoju: ");
            int poj = wczytajInt("Pojemnosc: ");
            double cena = wczytajDouble("Cena za dobe: ");
            int id = roomService.dodajPokoj(numer, poj, cena);
            std::cout << "Dodano pokoj, id=" << id << "\n";
        }
        else if (op == 2) {
            int id = wczytajInt("ID pokoju: ");
            std::cout << (roomService.usunPokoj(id) ? "OK\n" : "Nie znaleziono/nieaktywny\n");
        }
        else if (op == 3) {
            const std::vector<Pokoj>& lista = roomService.lista();
            for (size_t i = 0; i < lista.size(); i++) {
                const Pokoj& p = lista[i];
                std::cout << "ID=" << p.id
                    << " nr=" << p.numer
                    << " poj=" << p.pojemnosc
                    << " cena=" << p.cenaZaDobe
                    << " aktywny=" << (p.aktywny ? "tak" : "nie") << "\n";
            }
        }
    }
}

void menuRezerwacje(ReservationService& resService) {
    while (true) {
        std::cout << "\n== REZERWACJE ==\n"
            << "1) Utworz rezerwacje\n"
            << "2) Potwierdz rezerwacje\n"
            << "3) Anuluj rezerwacje\n"
            << "4) Lista rezerwacji\n"
            << "0) Wroc\n";
        int op = wczytajInt("> ");

        if (op == 0) return;

        if (op == 1) {
            int klientId = wczytajInt("Klient ID: ");
            int pokojId = wczytajInt("Pokoj ID: ");
            int od = wczytajInt("Od dnia (int): ");
            int doD = wczytajInt("Do dnia (int): ");
            int id = resService.utworzRezerwacje(klientId, pokojId, od, doD);
            std::cout << "Utworzono rezerwacje, id=" << id << "\n";
        }
        else if (op == 2) {
            int id = wczytajInt("ID rezerwacji: ");
            std::cout << (resService.potwierdz(id) ? "OK\n" : "Nie mozna potwierdzic\n");
        }
        else if (op == 3) {
            int id = wczytajInt("ID rezerwacji: ");
            std::cout << (resService.anuluj(id) ? "OK\n" : "Nie mozna anulowac\n");
        }
        else if (op == 4) {
            const std::vector<Rezerwacja>& lista = resService.lista();
            for (size_t i = 0; i < lista.size(); i++) {
                const Rezerwacja& r = lista[i];
                std::cout << "ID=" << r.id
                    << " klientId=" << r.klientId
                    << " pokojId=" << r.pokojId
                    << " [" << r.odDnia << "-" << r.doDnia << "]"
                    << " status=" << to_string(r.status) << "\n";
            }
        }
    }
}

int main() {
    RoomService roomService;
    ReservationService resService;

    while (true) {
        std::cout << "\n=== HOTEL APP (konsola) ===\n"
            << "1) Pokoje\n"
            << "2) Rezerwacje\n"
            << "0) Wyjscie\n";
        int op = wczytajInt("> ");

        if (op == 0) break;
        if (op == 1) menuPokoje(roomService);
        if (op == 2) menuRezerwacje(resService);
    }

    std::cout << "Koniec.\n";
    return 0;
}
