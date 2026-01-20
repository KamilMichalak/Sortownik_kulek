# Automatyczny Sortownik Kolorów

Projekt automatycznego sortownika, który rozpoznaje kolory kulek (Czerwony, Zielony, Niebieski) i rozdziela je do odpowiednich pojemników za pomocą serwomechanizmów. System wyposażony jest w algorytm autokalibracji balansu bieli oraz funkcję pauzy sprzętowej.

## Funkcjonalności

* **Rozpoznawanie kolorów:** Identyfikacja barw RGB przy użyciu czujnika TCS3200.
* **Logika sortowania:**
    * **Czerwony:** Zrzut na prawo.
    * **Zielony:** Zrzut na środek.
    * **Niebieski:** Zrzut na lewo.
* **Mechanika:** 3 serwomechanizmy.
* **Interfejs:** Sygnalizacja LED wykrytego koloru oraz trybu pauzy.
* **Pauza:** Fizyczny przycisk wstrzymujący pracę (z efektem wizualnym - wszystkie diody migają).
* **Autokalibracja:** Dynamiczne dostosowanie do oświetlenia przy starcie.

## Autorzy 
* **Kamil Michalak** (160127)
* **Mikołaj Guć** (160242)
