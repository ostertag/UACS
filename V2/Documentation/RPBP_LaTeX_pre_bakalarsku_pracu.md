# Ročníkový projekt a úvod do bakalárskej práce (RPBP)

## LaTeX pre bakalársku prácu (1)

---

## Kostra práce (1)

Stiahnite si kostru bakalárskej práce, ktorá obsahuje požadovaný formát prvých strán a ukážky užitočných príkazov.

- Kostra práce je z katedry informatiky, aktualizujte si ju podľa vášho štúdia.
- Upload do vášho konta na [overleaf.com](https://overleaf.com)

---

## Kostra práce (2)

Kostra obsahuje niekoľko súborov:

- `main.tex` — hlavný súbor, v ktorom je kostra práce
- `main-en.tex` — ukážka, ako adaptovať hlavný súbor pre práce v angličtine
- ďalšie súbory `.tex` — jednotlivé kapitoly práce
- `literatura.bib` — zoznam literatúry v BibTeXu
- priečinok `images` s obrázkami a zadaním v PDF formáte

---

## Členenie práce

Bakalárska práca je vlastne kniha, začína riadkom:

```latex
\documentclass[12pt, twoside]{book}
```

a tým pádom sa očakáva členenie na kapitoly, podkapitoly atď.

- Kapitola: `\chapter{Nazov}`
- Podkapitola: `\section{Nazov}`
- Ešte podrobnejšie členenie: `\subsection{Nazov}`
- Ak niečo nechceme číslovať, použijeme `*` (nebude to v obsahu):

```latex
\chapter*{Úvod}
\addcontentsline{toc}{chapter}{Úvod}
```

---

## Číslovanie a odkazy v rámci textu

LaTeX automaticky čísluje kapitoly a podkapitoly, obrázky, tabuľky, vzorce, definície, lémy a iné.

- Na jednotlivé súčasti textu je vhodné odkazovať sa číslom.
- Aby sme nemuseli čísla ručne meniť, použijeme dvojicu príkazov `\label` a `\ref`.

**Príklad — označenie kapitoly:**

```latex
\chapter{Implementácia}
\label{chap:impl}
```

**Príklad — odkaz na kapitolu v texte:**

```latex
Ako konkrétne sme algoritmus implementovali, vysvetlíme v kapitole \ref{chap:impl}.
```

---

## Obrázky a tabuľky (1)

- Vkladajú sa v „plávajúcich" (floating) prostrediach `figure` a `table`.
- LaTeX ich umiestni na vhodné miesto (vrch aktuálnej strany, prípadne ďalšej strany alebo aj samostatná strana).
- Po obrázok a tabuľku vkladáme aj popis (`caption`).
- LaTeX obrázky a tabuľky čísluje.
- Na každý obrázok a tabuľku by ste sa mali v texte aspoň raz odkázať (pomocou `\ref`).
- `\label` dajte až za `\caption`.

---

## Obrázok

```latex
\begin{figure}
  \begin{center}
    \includegraphics[width=0.5\textwidth]{obrazok.png}
    \caption{Príklad kostry grafu.}\label{fig:kostra}
  \end{center}
\end{figure}
```

---

## Tabuľka

```latex
\begin{table}
  \begin{center}
    \caption{A simple table}\label{tab:simple}
    \begin{tabular}{| l c r |}
      \hline
      1 & 2 & 3 \\
      4 & 5 & 6 \\
      7 & 8 & 9 \\
      \hline
    \end{tabular}
  \end{center}
\end{table}
```

---

## Práca s literatúrou

- V súbore `.bib` si spravíme zoznam použitých zdrojov, každému dáme label.
- Presný záznam vieme stiahnuť z niektorých stránok (napr. Google Scholar).
- V texte odkazujeme na zdroje pomocou `\cite{label}`.
- Každý zdroj musí byť minimálne raz citovaný.

---

## Práca s literatúrou — časté chyby

- **Veľké písmená v názvoch:** BibTeX mení písmená v názve článku na malé, čo je problém pri skratkách a vlastných menách. Vložte tieto slová do `{}`. Napr.:
  ```bibtex
  title = "Theano: a {CPU} and {GPU} math expression compiler"
  ```
- **Poradie mien autorov:** Mená autorov treba písať v poradí `krstné meno priezvisko` alebo `priezvisko, krstné meno` (t.j. `Jozef Mrkvička` alebo `Mrkvička, Jozef`).
- **Oddeľovanie autorov:** Autorov oddeľujte slovom `and`, čiarka sa považuje za oddelenie priezviska a mena.
- **Organizácia ako autor:** Ak je autorom organizácia s viacslovným názvom, dajte ju do `{}`, napr.:
  ```bibtex
  author = "{ENCODE Project Consortium}"
  ```
- **Veľa autorov:** Ak je autorov veľa, uveďte prvého a `et al.`, čo dostanete v BibTeXu pomocou `and others`.

---

## Zaujímavé balíčky

- **`subcaption`** — urobí do jedného obrázku viac podobrázkov.
- **`listings`** — zvýrazňovanie a formátovanie zdrojového kódu vkladaného do LaTeXu.
- [Návod na formátovanie pseudokódu](https://en.wikibooks.org/wiki/LaTeX/Algorithms)
- [Table Generator](https://www.tablesgenerator.com/) — stránka na generovanie LaTeXových tabuliek prekopírovaním tabuľky z kancelárskeho softvéru.

---

## Kontrola originality záverečnej práce

- Pre potreby CRZP: formát PDF, ktorý nesmie byť zaheslovaný a musí sa dať previesť na čistý text.
- Otestovanie prevodu na čistý text.
- Najnovšie verzie `pdflatex`-u vytvoria PDF súbor, ktorý je dobre prevediteľný na čistý text.
- Staršie verzie môžu mať problém s kódovaním diakritiky — pomoc: inštalácia balíčkov fontov `cm-super`.
# Ročníkový projekt a úvod do bakalárskej práce (RPBP)

## LaTeX pre bakalársku prácu (2)

---

## Poznámky pod čiarou

Pomocou príkazu `\footnote{text poznámky pod čiarou}` sa vytvorí na konci danej strany poznámka pod čiarou.

> Poznámky by mali byť vždy použité za slovom alebo za vetou, na ktorú sa vzťahujú.

**Príklad:**

```latex
Toto je veta s poznámkou.\footnote{Tu je text poznámky pod čiarou.}
```

---

## Zvýraznené slová

Dôležité slová zvýrazňujeme podčiarknutím, kurzívou alebo tučným písmom.

| Efekt | Príkaz |
|---|---|
| *Kurzíva* | `\emph{text}` |
| **Tučné písmo** | `\textbf{text}` |
| Podčiarknuté | `\underline{text}` |

---

## Prostredia

Pre napísanie textu rôzneho typu, LaTeX má definované množstvo prostredí na rôzne druhy formátovania.

Základná syntax prostredia:

```latex
\begin{nazov}
  text
\end{nazov}
```

Prostredia môžu byť volané viackrát (aj jedno v druhom), pokiaľ je dodržané ich poradie:

```latex
\begin{aaa}
  ...
  \begin{bbb}
    ...
  \end{bbb}
  ...
\end{aaa}
```

---

## Odrážky, číslované zoznamy a popisy

- Prostredie `itemize` — jednoduché odrážky
- Prostredie `enumerate` — vytváranie číslovaných zoznamov
- Prostredie `description` — vytváranie popisov

**Príklad (kód a výsledok):**

```latex
\begin{enumerate}
\item Prostredia môžete kombinovať podľa chuti:
  \begin{itemize}
    \item Ale môže to začať vyzerať zle.
    \item[-] S pomlčkou.
  \end{itemize}
\item Preto si pamätajte:
  \begin{description}
    \item[Hlúposti] sa nestanú rozum\-ný\-mi preto,
      že budú v~zozname.
    \item[Rozumné] veci však môžu byť podané v~zozname.
  \end{description}
\end{enumerate}
```

Výsledok:

1. Prostredia môžete kombinovať podľa chuti:
   - Ale môže to začať vyzerať zle.
   - S pomlčkou.
2. Preto si pamätajte:
   - **Hlúposti** sa nestanú rozumnými preto, že budú v zozname.
   - **Rozumné** veci však môžu byť podané v zozname.

---

## Matematický text

Matematický text je v odseku (inline) písaný jedným z týchto spôsobov:

```latex
\begin{math} výraz \end{math}

$výraz$

\( výraz \)
```

Všetky tri zápisy sú ekvivalentné. Odporúčaná forma je `$...$` alebo `\(...\)`.

---

## Matematické vzorce

**Nečíslované** — prostredie `displaymath`:

```latex
\begin{displaymath}
  výraz
\end{displaymath}
```

**Číslované** — prostredie `equation`:

```latex
\begin{equation}
  výraz
  \label{eq:moj_vzorec}
\end{equation}
```

Pomocou `\label` a `\ref` sa v texte môžeme na rovnice odvolať:

```latex
Rovnica~\ref{eq:moj_vzorec} ukazuje...
```

---

## Rôzne zobrazenia

Výrazy budú vytlačené rôznym spôsobom, ak budú rôzne zadané.

**Inline (v odseku) — `$...$`:**

```latex
$\lim_{n \to \infty} \sum_{k=1}^n \frac{1}{k^2} = \frac{\pi^2}{6}$
```

Výsledok: $\lim_{n \to \infty} \sum_{k=1}^n \frac{1}{k^2} = \frac{\pi^2}{6}$ *(kompaktné, indexy vedľa seba)*

**Display (na vlastnom riadku) — `displaymath`:**

```latex
\begin{displaymath}
\lim_{n \to \infty} \sum_{k=1}^n \frac{1}{k^2} = \frac{\pi^6}{6}
\end{displaymath}
```

Výsledok — veľký, rozvinutý vzorec na vlastnom riadku:

$$\lim_{n \to \infty} \sum_{k=1}^{n} \frac{1}{k^2} = \frac{\pi^2}{6}$$

---

## Matematický režim — rozdiely

- Väčšina medzier a ukončení riadkov **nemá význam**; všetky medzery sú buď odvodené z logického obsahu matematických výrazov, alebo musia byť zadané pomocou príkazov.
- Prázdne riadky **nie sú dovolené** — jeden odsek, jeden výraz.
- Každé písmeno sa považuje za názov premennej a je vysádzané ako premenná (*kurzívou*). Ak chcete vo výraze použiť normálny text, musíte použiť príkaz:

```latex
\textrm{normálny text vo vzorci}
```

---

## Sádzanie matematických výrazov

Pre podrobný prehľad všetkých matematických príkazov a prostredí odporúčame:

> **LaTeX za 95 minút** — stručný a praktický úvod do LaTeXu.
