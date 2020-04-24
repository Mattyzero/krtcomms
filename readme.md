# KRTCommander

Dieses Projekt benutzt nicht qmake

## Qt Installieren

Qt wurde als Open Source Variante installiert und kann [hier](https://www.qt.io/download-open-source) heruntergeladen werden.

Falls Qt nicht unter C:\Qt installiert ist müssen die .props files im props Ordner angepasst werden

## Automatisch generierte Dateien

Alle \*\_moc.cpp Dateien werden automatisch mit $(QtPath)\bin\moc.exe  aus den Vorhandenen Header-Files die UI Elemente beinhalten kompiliert.
Alle ui\_\*.h Dateien werden automatisch mit $(QtPath)\bin\uic.exe aus den Vorhandenen \*.ui Dateien kompiliert.

# Sonstiges

Ja ich weiß es ist kein Cleanes Projekt wenn wer bock zum Aufräumen hat mach nen PR