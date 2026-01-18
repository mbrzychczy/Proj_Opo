#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>

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

// ====== Enum statusu pokoju ======
enum class RoomStatus {
    Wolny,
    Zajety,
    Niedostepny
};

std::string to_string(RoomStatus s) {
    switch (s) {
    case RoomStatus::Wolny:        return "wolny";
    case RoomStatus::Zajety:       return "zajety";
    case RoomStatus::Niedostepny:  return "niedostepny";
    }
    return "unknown";
}

// ====== Enum statusu płatności ======
enum class PaymentStatus {
    WTrakcie,
    Oplacone,
    Anulowane
};

std::string to_string(PaymentStatus s) {
    switch (s) {
    case PaymentStatus::WTrakcie:  return "w_trakcie";
    case PaymentStatus::Oplacone:  return "oplacone";
    case PaymentStatus::Anulowane: return "anulowane";
    }
    return "unknown";
}

// ====== Enum metody płatności ======
enum class PaymentMethodType {
    Gotowka,
    Karta,
    Przelew
};

std::string to_string(PaymentMethodType m) {
    switch (m) {
    case PaymentMethodType::Gotowka: return "gotowka";
    case PaymentMethodType::Karta:   return "karta";
    case PaymentMethodType::Przelew: return "przelew";
    }
    return "unknown";
}

enum class UserRole {
    None,
    Admin,
    Recepcjonista,
    Klient
};


// ====== Modele ======
struct Pokoj {
    int id;
    int numer;
    int pojemnosc;
    double cenaZaDobe;
    bool aktywny = true;
    RoomStatus status = RoomStatus::Wolny;
    std::string opis;
};

struct Klient {
    int id;
    std::string pesel;
    std::string imie;
    std::string nazwisko;
    std::string email;
};

struct Rezerwacja {
    int id;
    int klientId;
    int pokojId;
    int odDnia;
    int doDnia;
    BookingStatus status = BookingStatus::Utworzona;
    int platnoscId = -1;
};

struct Platnosc {
    int id;
    int rezerwacjaId;
    int kwota;
    PaymentStatus status = PaymentStatus::WTrakcie;
    PaymentMethodType metoda;
};

// ====== Obserwatorzy rezerwacji ======
struct RezerwacjaObserwacja {
    virtual ~RezerwacjaObserwacja() {}
    virtual void update(BookingStatus s) = 0;
};

struct KlientObserwator : public RezerwacjaObserwacja {
    void update(BookingStatus s) override {
        std::cout << "[Klient] Nowy status rezerwacji: " << to_string(s) << "\n";
    }
};

struct RecepcjonistaObserwator : public RezerwacjaObserwacja {
    void update(BookingStatus s) override {
        std::cout << "[Recepcjonista] Status rezerwacji: " << to_string(s) << "\n";
    }
};

struct SystemPowiadomien : public RezerwacjaObserwacja {
    void update(BookingStatus s) override {
        std::cout << "[SystemPowiadomien] Zapisano status: " << to_string(s) << "\n";
    }
};

// ====== RezerwacjaDane (obiekt z obserwatorami) ======
struct RezerwacjaDane {
    std::string idRezerwacji;
    int pokojId;
    int kwota;
    BookingStatus status;
    std::vector<RezerwacjaObserwacja*> obserwatorzy;

    void przypisz(RezerwacjaObserwacja* o) {
        obserwatorzy.push_back(o);
    }

    void wypisz(RezerwacjaObserwacja* o) {
        for (size_t i = 0; i < obserwatorzy.size(); i++) {
            if (obserwatorzy[i] == o) {
                obserwatorzy.erase(obserwatorzy.begin() + i);
                break;
            }
        }
    }

    void powiadom() {
        for (size_t i = 0; i < obserwatorzy.size(); i++) {
            obserwatorzy[i]->update(status);
        }
    }

    void ustawStatus(BookingStatus s) {
        status = s;
        powiadom();
    }
};

// ====== RezerwacjaBuilder ======
class RezerwacjaBuilder {
    RezerwacjaDane dane;
public:
    RezerwacjaBuilder& setId(const std::string& id) {
        dane.idRezerwacji = id;
        return *this;
    }
    RezerwacjaBuilder& setPokoj(int pokojId) {
        dane.pokojId = pokojId;
        return *this;
    }
    RezerwacjaBuilder& setKwota(int kwota) {
        dane.kwota = kwota;
        return *this;
    }
    RezerwacjaBuilder& setStatus(BookingStatus s) {
        dane.status = s;
        return *this;
    }
    RezerwacjaDane build() {
        return dane;
    }
};

// ====== RoomService ======
class RoomService {
    std::vector<Pokoj> pokoje;
    int nextId = 1;
public:
    int dodajPokoj(int numer, int pojemnosc, double cenaZaDobe, const std::string& opis) {
        Pokoj p;
        p.id = nextId++;
        p.numer = numer;
        p.pojemnosc = pojemnosc;
        p.cenaZaDobe = cenaZaDobe;
        p.aktywny = true;
        p.status = RoomStatus::Wolny;
        p.opis = opis;
        pokoje.push_back(p);
        return p.id;
    }

    bool usunPokoj(int id) {
        for (size_t i = 0; i < pokoje.size(); i++) {
            if (pokoje[i].id == id && pokoje[i].aktywny) {
                pokoje[i].aktywny = false;
                pokoje[i].status = RoomStatus::Niedostepny;
                return true;
            }
        }
        return false;
    }

    bool edytujPokoj(int id, const std::string& nowyOpis, double nowaCena) {
        for (size_t i = 0; i < pokoje.size(); i++) {
            if (pokoje[i].id == id && pokoje[i].aktywny) {
                pokoje[i].opis = nowyOpis;
                pokoje[i].cenaZaDobe = nowaCena;
                return true;
            }
        }
        return false;
    }

    const std::vector<Pokoj>& lista() const {
        return pokoje;
    }

    Pokoj* znajdzMutable(int id) {
        for (size_t i = 0; i < pokoje.size(); i++) {
            if (pokoje[i].id == id) return &pokoje[i];
        }
        return nullptr;
    }

    const Pokoj* znajdz(int id) const {
        for (size_t i = 0; i < pokoje.size(); i++) {
            if (pokoje[i].id == id) return &pokoje[i];
        }
        return nullptr;
    }

    bool ustawStatus(int id, RoomStatus s) {
        Pokoj* p = znajdzMutable(id);
        if (!p) return false;
        p->status = s;
        return true;
    }

    bool jestWolny(int id, int /*od*/, int /*doDnia*/) const {
        const Pokoj* p = znajdz(id);
        if (!p) return false;
        if (!p->aktywny) return false;
        return p->status == RoomStatus::Wolny;
    }
};

// ====== PaymentService ======
class PaymentService {
    std::vector<Platnosc> platnosci;
    int nextId = 1;
public:
    int utworzPlatnosc(int rezerwacjaId, int kwota, PaymentMethodType metoda) {
        Platnosc p;
        p.id = nextId++;
        p.rezerwacjaId = rezerwacjaId;
        p.kwota = kwota;
        p.status = PaymentStatus::WTrakcie;
        p.metoda = metoda;
        platnosci.push_back(p);
        return p.id;
    }

    bool anulujPlatnosc(int id) {
        for (size_t i = 0; i < platnosci.size(); i++) {
            if (platnosci[i].id == id && platnosci[i].status != PaymentStatus::Anulowane) {
                platnosci[i].status = PaymentStatus::Anulowane;
                return true;
            }
        }
        return false;
    }

    bool oznaczOplacone(int id) {
        for (size_t i = 0; i < platnosci.size(); i++) {
            if (platnosci[i].id == id && platnosci[i].status == PaymentStatus::WTrakcie) {
                platnosci[i].status = PaymentStatus::Oplacone;
                return true;
            }
        }
        return false;
    }

    const std::vector<Platnosc>& lista() const {
        return platnosci;
    }

    const Platnosc* znajdz(int id) const {
        for (size_t i = 0; i < platnosci.size(); i++) {
            if (platnosci[i].id == id) return &platnosci[i];
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
        r.status = BookingStatus::Oczekujaca;
        r.platnoscId = -1;
        rezerwacje.push_back(r);
        return r.id;
    }

    bool powiazPlatnosc(int rezerwacjaId, int platnoscId) {
        for (size_t i = 0; i < rezerwacje.size(); i++) {
            if (rezerwacje[i].id == rezerwacjaId) {
                rezerwacje[i].platnoscId = platnoscId;
                return true;
            }
        }
        return false;
    }

    bool potwierdz(int id) {
        for (size_t i = 0; i < rezerwacje.size(); i++) {
            if (rezerwacje[i].id == id &&
                (rezerwacje[i].status == BookingStatus::Utworzona ||
                    rezerwacje[i].status == BookingStatus::Oczekujaca)) {
                rezerwacje[i].status = BookingStatus::Potwierdzona;
                return true;
            }
        }
        return false;
    }

    bool anuluj(int id) {
        for (size_t i = 0; i < rezerwacje.size(); i++) {
            if (rezerwacje[i].id == id &&
                rezerwacje[i].status != BookingStatus::Anulowana &&
                rezerwacje[i].status != BookingStatus::Wykonana) {
                rezerwacje[i].status = BookingStatus::Anulowana;
                return true;
            }
        }
        return false;
    }

    bool zamelduj(int id, RoomService& roomService) {
        for (size_t i = 0; i < rezerwacje.size(); i++) {
            if (rezerwacje[i].id == id &&
                rezerwacje[i].status == BookingStatus::Potwierdzona) {
                rezerwacje[i].status = BookingStatus::Zameldowany;
                roomService.ustawStatus(rezerwacje[i].pokojId, RoomStatus::Zajety);
                return true;
            }
        }
        return false;
    }

    bool wymelduj(int id, RoomService& roomService) {
        for (size_t i = 0; i < rezerwacje.size(); i++) {
            if (rezerwacje[i].id == id &&
                (rezerwacje[i].status == BookingStatus::Zameldowany ||
                    rezerwacje[i].status == BookingStatus::Potwierdzona)) {
                rezerwacje[i].status = BookingStatus::Wykonana;
                roomService.ustawStatus(rezerwacje[i].pokojId, RoomStatus::Wolny);
                return true;
            }
        }
        return false;
    }

    bool sprawdzDostepnosc(int pokojId, int odDnia, int doDnia, const RoomService& roomService) const {
        if (!roomService.jestWolny(pokojId, odDnia, doDnia)) return false;
        for (size_t i = 0; i < rezerwacje.size(); i++) {
            const Rezerwacja& r = rezerwacje[i];
            if (r.pokojId == pokojId &&
                r.status != BookingStatus::Anulowana &&
                r.status != BookingStatus::Wykonana) {
                if (!(doDnia <= r.odDnia || odDnia >= r.doDnia)) {
                    return false;
                }
            }
        }
        return true;
    }

    const std::vector<Rezerwacja>& lista() const {
        return rezerwacje;
    }
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

std::string wczytajStringLine(const std::string& prompt) {
    std::cout << prompt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string s;
    std::getline(std::cin, s);
    return s;
}

// ====== Menu: Pokoje ======
void menuPokoje(RoomService& roomService) {
    while (true) {
        std::cout << "\n== POKOJE ==\n"
            << "1) Dodaj pokoj\n"
            << "2) Usun (dezaktywuj) pokoj\n"
            << "3) Lista pokoi\n"
            << "4) Edytuj pokoj (opis, cena)\n"
            << "0) Wroc\n";
        int op = wczytajInt("> ");
        if (op == 0) return;

        if (op == 1) {
            int numer = wczytajInt("Numer pokoju: ");
            int poj = wczytajInt("Pojemnosc: ");
            double cena = wczytajDouble("Cena za dobe: ");
            std::string opis = wczytajStringLine("Opis pokoju: ");
            int id = roomService.dodajPokoj(numer, poj, cena, opis);
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
                    << " status=" << to_string(p.status)
                    << " aktywny=" << (p.aktywny ? "tak" : "nie")
                    << " opis=\"" << p.opis << "\""
                    << "\n";
            }
        }
        else if (op == 4) {
            int id = wczytajInt("ID pokoju: ");
            std::string opis = wczytajStringLine("Nowy opis: ");
            double cena = wczytajDouble("Nowa cena: ");
            std::cout << (roomService.edytujPokoj(id, opis, cena) ? "Zaktualizowano\n" : "Blad\n");
        }
    }
}

// ====== Menu: Rezerwacje ======
void menuRezerwacje(ReservationService& resService, RoomService& roomService, PaymentService& payService) {
    while (true) {
        std::cout << "\n== REZERWACJE ==\n"
            << "1) Utworz rezerwacje\n"
            << "2) Potwierdz rezerwacje\n"
            << "3) Anuluj rezerwacje\n"
            << "4) Lista rezerwacji\n"
            << "5) Zamelduj\n"
            << "6) Wymelduj\n"
            << "7) Sprawdz dostepnosc pokoju\n"
            << "8) Utworz platnosc do rezerwacji\n"
            << "0) Wroc\n";
        int op = wczytajInt("> ");
        if (op == 0) return;

        if (op == 1) {
            int klientId = wczytajInt("Klient ID (dowolna liczba): ");
            int pokojId = wczytajInt("Pokoj ID: ");
            int od = wczytajInt("Od dnia (int): ");
            int doD = wczytajInt("Do dnia (int): ");
            if (!resService.sprawdzDostepnosc(pokojId, od, doD, roomService)) {
                std::cout << "Pokoj niedostepny w tym terminie.\n";
            }
            else {
                int id = resService.utworzRezerwacje(klientId, pokojId, od, doD);
                std::cout << "Utworzono rezerwacje, id=" << id << "\n";
            }
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
                    << " status=" << to_string(r.status)
                    << " platnoscId=" << r.platnoscId
                    << "\n";
            }
        }
        else if (op == 5) {
            int id = wczytajInt("ID rezerwacji: ");
            std::cout << (resService.zamelduj(id, roomService) ? "Zameldowano\n" : "Nie mozna zameldowac\n");
        }
        else if (op == 6) {
            int id = wczytajInt("ID rezerwacji: ");
            std::cout << (resService.wymelduj(id, roomService) ? "Wymeldowano\n" : "Nie mozna wymeldowac\n");
        }
        else if (op == 7) {
            int pokojId = wczytajInt("Pokoj ID: ");
            int od = wczytajInt("Od dnia (int): ");
            int doD = wczytajInt("Do dnia (int): ");
            bool ok = resService.sprawdzDostepnosc(pokojId, od, doD, roomService);
            std::cout << (ok ? "Dostepny\n" : "Niedostepny\n");
        }
        else if (op == 8) {
            int rezId = wczytajInt("ID rezerwacji: ");
            int kwota = wczytajInt("Kwota (int): ");
            std::cout << "Metoda platnosci: 1) gotowka 2) karta 3) przelew\n";
            int m = wczytajInt("> ");
            PaymentMethodType metoda = PaymentMethodType::Gotowka;
            if (m == 2) metoda = PaymentMethodType::Karta;
            else if (m == 3) metoda = PaymentMethodType::Przelew;
            int pid = payService.utworzPlatnosc(rezId, kwota, metoda);
            resService.powiazPlatnosc(rezId, pid);
            std::cout << "Utworzono platnosc, id=" << pid << "\n";
        }
    }
}

// ====== Menu: Platnosci ======
void menuPlatnosci(PaymentService& payService) {
    while (true) {
        std::cout << "\n== PLATNOSCI ==\n"
            << "1) Lista platnosci\n"
            << "2) Oznacz jako oplacona\n"
            << "3) Anuluj platnosc\n"
            << "0) Wroc\n";
        int op = wczytajInt("> ");
        if (op == 0) return;

        if (op == 1) {
            const std::vector<Platnosc>& lista = payService.lista();
            for (size_t i = 0; i < lista.size(); i++) {
                const Platnosc& p = lista[i];
                std::cout << "ID=" << p.id
                    << " rezerwacjaId=" << p.rezerwacjaId
                    << " kwota=" << p.kwota
                    << " metoda=" << to_string(p.metoda)
                    << " status=" << to_string(p.status)
                    << "\n";
            }
        }
        else if (op == 2) {
            int id = wczytajInt("ID platnosci: ");
            std::cout << (payService.oznaczOplacone(id) ? "OK\n" : "Nie mozna oznaczyc\n");
        }
        else if (op == 3) {
            int id = wczytajInt("ID platnosci: ");
            std::cout << (payService.anulujPlatnosc(id) ? "OK\n" : "Nie mozna anulowac\n");
        }
    }
}


UserRole menuLogowanie() {
    while (true) {
        std::cout << "\n=== LOGOWANIE ===\n"
            << "1) Administrator\n"
            << "2) Recepcjonista\n"
            << "3) Klient\n"
            << "0) Wyjscie\n";

        int op = wczytajInt("> ");
        if (op == 0) return UserRole::None;
        if (op == 1) return UserRole::Admin;
        if (op == 2) return UserRole::Recepcjonista;
        if (op == 3) return UserRole::Klient;

        std::cout << "Niepoprawny wybor.\n";
    }
}


void menuAdmin(RoomService& roomService, ReservationService& resService) {
    while (true) {
        std::cout << "\n=== ADMINISTRATOR ===\n"
            << "1) Zarzadzanie pokojami\n"
            << "2) Generuj raport\n"
            << "0) Wyloguj\n";

        int op = wczytajInt("> ");
        if (op == 0) return;

        if (op == 1) menuPokoje(roomService);
        else if (op == 2) {
            std::cout << "\n--- RAPORT ---\n";
            std::cout << "Pokoje:\n";
            for (auto& p : roomService.lista()) {
                std::cout << "Pokoj " << p.numer << " status=" << to_string(p.status) << "\n";
            }
            std::cout << "\nRezerwacje:\n";
            for (auto& r : resService.lista()) {
                std::cout << "Rezerwacja " << r.id << " status=" << to_string(r.status) << "\n";
            }
        }
    }
}


void menuRecepcjonista(RoomService& roomService,
    ReservationService& resService,
    PaymentService& payService)
{
    while (true) {
        std::cout << "\n=== RECEPCJONISTA ===\n"
            << "1) Lista pokoi\n"
            << "2) Lista rezerwacji\n"
            << "3) Potwierdz rezerwacje\n"
            << "4) Zamelduj\n"
            << "5) Wymelduj\n"
            << "6) Platnosci\n"
            << "0) Wyloguj\n";

        int op = wczytajInt("> ");
        if (op == 0) return;

        if (op == 1) {
            for (auto& p : roomService.lista()) {
                std::cout << "Pokoj " << p.numer << " status=" << to_string(p.status) << "\n";
            }
        }
        else if (op == 2) {
            for (auto& r : resService.lista()) {
                std::cout << "Rezerwacja " << r.id << " status=" << to_string(r.status) << "\n";
            }
        }
        else if (op == 3) {
            int id = wczytajInt("ID rezerwacji: ");
            std::cout << (resService.potwierdz(id) ? "OK\n" : "Blad\n");
        }
        else if (op == 4) {
            int id = wczytajInt("ID rezerwacji: ");
            std::cout << (resService.zamelduj(id, roomService) ? "Zameldowano\n" : "Blad\n");
        }
        else if (op == 5) {
            int id = wczytajInt("ID rezerwacji: ");
            std::cout << (resService.wymelduj(id, roomService) ? "Wymeldowano\n" : "Blad\n");
        }
        else if (op == 6) {
            menuPlatnosci(payService);
        }
    }
}


void menuKlient(RoomService& roomService,
    ReservationService& resService,
    PaymentService& payService)
{
    while (true) {
        std::cout << "\n=== KLIENT ===\n"
            << "1) Przegladaj pokoje\n"
            << "2) Utworz rezerwacje\n"
            << "3) Anuluj rezerwacje\n"
            << "4) Zaplac za rezerwacje\n"
            << "0) Wyloguj\n";

        int op = wczytajInt("> ");
        if (op == 0) return;

        if (op == 1) {
            for (auto& p : roomService.lista()) {
                if (p.aktywny)
                    std::cout << "Pokoj " << p.numer << " cena=" << p.cenaZaDobe << "\n";
            }
        }
        else if (op == 2) {
            int klientId = wczytajInt("Twoje ID: ");
            int pokojId = wczytajInt("Pokoj ID: ");
            int od = wczytajInt("Od dnia: ");
            int doD = wczytajInt("Do dnia: ");

            if (!resService.sprawdzDostepnosc(pokojId, od, doD, roomService))
                std::cout << "Pokoj zajety.\n";
            else {
                int id = resService.utworzRezerwacje(klientId, pokojId, od, doD);
                std::cout << "Utworzono rezerwacje ID=" << id << "\n";
            }
        }
        else if (op == 3) {
            int id = wczytajInt("ID rezerwacji: ");
            std::cout << (resService.anuluj(id) ? "Anulowano\n" : "Blad\n");
        }
        else if (op == 4) {
            int rezId = wczytajInt("ID rezerwacji: ");
            int kwota = wczytajInt("Kwota: ");
            std::cout << "Metoda: 1) gotowka 2) karta 3) przelew\n";
            int m = wczytajInt("> ");

            PaymentMethodType metoda = PaymentMethodType::Gotowka;
            if (m == 2) metoda = PaymentMethodType::Karta;
            else if (m == 3) metoda = PaymentMethodType::Przelew;

            int pid = payService.utworzPlatnosc(rezId, kwota, metoda);
            resService.powiazPlatnosc(rezId, pid);
            payService.oznaczOplacone(pid);

            std::cout << "Platnosc zrealizowana.\n";
        }
    }
}

// ====== main ======
int main() {
    RoomService roomService;
    ReservationService resService;
    PaymentService payService;

    while (true) {
        UserRole rola = menuLogowanie();
        if (rola == UserRole::None) break;

        if (rola == UserRole::Admin)
            menuAdmin(roomService, resService);

        else if (rola == UserRole::Recepcjonista)
            menuRecepcjonista(roomService, resService, payService);

        else if (rola == UserRole::Klient)
            menuKlient(roomService, resService, payService);
    }

    std::cout << "Koniec.\n";
    return 0;
}
