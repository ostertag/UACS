REM Obsah adresára je možné vymazať. Celý sa generuje týmto skriptom.
REM Na aktualizáciu panelu stačí tento skript znovu spustiť.

kikit panelize -p jlcpcb-reader.json reader.kicad_pcb panel\panel.kicad_pcb
kikit fab jlcpcb --schematic reader.kicad_sch panel\panel.kicad_pcb panel\
