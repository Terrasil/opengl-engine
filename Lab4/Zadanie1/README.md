### Zadanie 1

Stwórz scenę 3D z kilkoma (co najmniej 4) różnymi obiektami przedstawiającą fragment fikcyjnego świata złożony z podłoża i obiektów na nim. Jeśli masz inny pomysł z użyciem wielu obiektów, śmiało go zrealizuj. W tym zadaniu najważniejsze jest przećwiczenie ładowania plików OBJ oraz umieszczania ich w różnych miejscach sceny.

- Skorzystaj z projektu objfile i wczytaj gotowe obiekty z formatu OBJ z katalogu models lub własne.
- Dodaj możliwość oglądania sceny z różnych stron (obracania klawiszami WSAD) oraz oddalania i przybliżania widoku (klawiszami +-).
- Spraw, aby obiekty odróżniały się od siebie (np. różne potoki albo zmienne jednorodne kontrolujące sposób renderowania poszczególnych obiektów). Przykładowo, spraw aby podłoże było zielone/brązowe, a obiekty innego koloru. Wykorzystaj przejścia gradientowe, aby obiekty nie były jednolite.
- Niech co najmniej dwa obiekty na scenie zmieniają się w dowolny sposób, na przykład niech się obracają lub zmieniają kolory, czyli stwórz dowolną animację.

Wczytywanie i przetwarzanie wielu obiektów 3D może powodować bałagan w kodzie. Kopiowanie fragmentów kodu i nazywanie kolejnych zmiennych idVAO, idVAO1, idVAO2 jest bardzo nieeleganckie. Pomyśl o napisaniu funkcji createVAO(), createObject() lub innych, które wyeliminują konieczność kopiowania kodu służącego wczytywaniu plików OBJ, tworzeniu VAO i buforów. A jeszcze lepszym pomysłem byłoby utworzenie klasy CMesh z metodami do wczytywania pliku OBJ, tworzenia VAO i buforów. Podobnie można utworzyć klasę CProgram do obsługi potoku graficznego. Wskazówki w pliku: wskazowki-obiektowo.cpp.

#### Wynik

![Screen1](Screen1.gif)

Na screenie widać obracjącą się scenę (teren oraz modele drzew i trawy).