### Zadanie 3

Napisz program, który wyświetla na ekranie domek wygenerowany w Blenderze i wyeksportowany do tablicy współrzędnych wierzchołków house.h. Popracuj nad shaderem fragmentów, aby domek ożył i wyglądał ciekawie (przykład).

Wskazówki:

- Dołącz plik house.h do swojego programu i potraktuj tak samo jak trójkąty w zadaniach z wykładu.
- Blender ma inny układ współrzędnych i może się okazać, że domek będzie odwrócony - jeśli tak, to poeksperymentuj ze współrzędnymi w vertex shaderze. Możesz tam dowolnie zmieniać ich kolejność.
- W shaderze fragmentów skorzystaj z gl_PrimitiveID oraz gl_FragCoord aby nadać inne kolory innym trójkątom.

#### Wynik

![Screen1](Screen1.gif)

Na animacji widać wyrenderowany domek z zadania 5 (lab 1) który zmienia kolory