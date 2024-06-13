#include <iostream>
#include <unordered_map>
#include <string>
#include <thread>
#include <chrono>
#include <random>

class Kullanici {
public:
    std::string kullaniciAdi;
    std::string anahtar;
    int kalanDeneme;
    int basariliDeneme;
    int basarisizDeneme;
    const int maxDeneme = 3;

    Kullanici(const std::string& adi, const std::string& key) 
        : kullaniciAdi(adi), anahtar(key), kalanDeneme(maxDeneme), basariliDeneme(0), basarisizDeneme(0) {}
};

class KeyAuthSystem {
private:
    std::unordered_map<std::string, Kullanici> kullaniciMap;

public:
    void kullaniciEkle(const std::string& kullaniciAdi, const std::string& anahtar) {
        kullaniciMap[kullaniciAdi] = Kullanici(kullaniciAdi, anahtar);
    }

    bool anahtarGecerliMi(const std::string& kullaniciAdi, const std::string& anahtar) {
        auto it = kullaniciMap.find(kullaniciAdi);
        if (it != kullaniciMap.end() && it->second.anahtar == anahtar) {
            return true;
        }
        return false;
    }

    void denemeAzalt(Kullanici& kullanici) {
        kullanici.kalanDeneme--;
        kullanici.basarisizDeneme++;
    }

    void basariliDenemeArttir(Kullanici& kullanici) {
        kullanici.basariliDeneme++;
    }

    void denemeSifirla(Kullanici& kullanici) {
        kullanici.kalanDeneme = kullanici.maxDeneme;
    }

    void beklet() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(3, 10); // 3 ile 10 saniye arasında rastgele bekleme süresi
        int beklemeSuresi = dis(gen);
        std::cout << "Çok fazla başarısız deneme. Lütfen " << beklemeSuresi << " saniye bekleyin." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(beklemeSuresi));
    }

    void denemeLogla(const std::string& kullaniciAdi, const std::string& anahtar, bool basarili) const {
        std::cout << "Kullanıcı '" << kullaniciAdi << "' tarafından yapılan deneme '" 
                  << anahtar << "' anahtarı ile " 
                  << (basarili ? "başarılı." : "başarısız.") << std::endl;
    }

    void epostaBildirim(const std::string& kullaniciAdi) const {
        std::cout << "Kullanıcı '" << kullaniciAdi << "' çok fazla başarısız deneme yaptı. Bildirim gönderildi." << std::endl;
    }

    void oturumOzet(Kullanici& kullanici) const {
        std::cout << "Oturum Özeti - Kullanıcı: " << kullanici.kullaniciAdi << std::endl;
        std::cout << "Başarılı denemeler: " << kullanici.basariliDeneme << std::endl;
        std::cout << "Başarısız denemeler: " << kullanici.basarisizDeneme << std::endl;
    }

    Kullanici* kullaniciBul(const std::string& kullaniciAdi) {
        auto it = kullaniciMap.find(kullaniciAdi);
        if (it != kullaniciMap.end()) {
            return &(it->second);
        }
        return nullptr;
    }
};

int main() {
    KeyAuthSystem authSystem;

    authSystem.kullaniciEkle("ali", "pleasantbabba");
    authSystem.kullaniciEkle("veli", "veliamca24234");
    authSystem.kullaniciEkle("ayse", "aysekizdsf1e2");

    std::string kullaniciAdi, kullaniciAnahtari;
    Kullanici* aktifKullanici = nullptr;

    std::cout << "Kullanıcı adınızı girin: ";
    std::cin >> kullaniciAdi;

    aktifKullanici = authSystem.kullaniciBul(kullaniciAdi);

    if (!aktifKullanici) {
        std::cout << "Geçersiz kullanıcı adı." << std::endl;
        return 0;
    }

    do {
        std::cout << "Anahtarınızı girin: ";
        std::cin >> kullaniciAnahtari;

        if (authSystem.anahtarGecerliMi(kullaniciAdi, kullaniciAnahtari)) {
            authSystem.basariliDenemeArttir(*aktifKullanici);
            std::cout << "Anahtar geçerli. Giriş izni verildi." << std::endl;
            authSystem.denemeLogla(kullaniciAdi, kullaniciAnahtari, true);
            break;
        } else {
            authSystem.denemeLogla(kullaniciAdi, kullaniciAnahtari, false);
            std::cout << "Geçersiz anahtar. Kalan deneme hakkı: " << aktifKullanici->kalanDeneme - 1 << std::endl;
            authSystem.denemeAzalt(*aktifKullanici);

            if (aktifKullanici->kalanDeneme == 0) {
                authSystem.epostaBildirim(kullaniciAdi);
                authSystem.beklet();
                authSystem.denemeSifirla(*aktifKullanici);
            }
        }
    } while (true);

    authSystem.oturumOzet(*aktifKullanici);

    return 0;
}
