### Zadanie 3

Zaimplementuj instanced rendering i stwórz na swojej scenie mnóstwo małych listków spadających z nieba, które dodatkowo się obracają. To tylko przykład - możesz użyć innego, który przyda Ci się do końcowego projektu (komary, trawa, kwiaty itp.). Wskazówki: wyszukaj przezroczystą teksturę liścia i nałóż na kwadrat tak jak w przykładzie z kwiatkiem z poprzednich wykładów. Nie musi to być liść. Ważne, aby było tego dużo i się poruszało. Zadbaj, aby obiekty miały różne położenie (np. losowe), różną orientację i wielkość. Aby wygodnie operować na obiektach, które mają jedną instancję (pojedyncze obiekty) oraz wiele instancji (tak jak np. te listki) warto pomyśleć o stworzeniu klasy CMultipleMesh obok naszej CMesh, która dziedziczyłaby po CMesh i udostępniała dodatkowe metody/rozwinięcie konstruktory z takimi parametrami jak liczba instancji.

#### Wynik

![Screen1](Screen1.gif)

Na screenie widać scenę gdzie symulowany jest opadający śnieg za pomocą multimesha.