Un Raspberry Pi è un computer delle dimensioni di una carta di credito su cui si può installare Polyphone. Integrandolo in un contenitore con altoparlanti e abbinandolo a una master keyboard, si può creare un vero sintetizzatore!

![Sintetizzatore fatto in casa (fronte)](images/synth_front.jpg "Sintetizzatore fatto in casa (fronte)")

![Sintetizzatore fatto in casa (retro)](images/synth_back.jpg "Sintetizzatore fatto in casa (retro)")

## Materiale necessario

Questo elenco può essere adattato al tuo progetto:

- un Raspberry Pi,
- una scheda micro SD da almeno 32 GB,
- un alimentatore per il Raspberry Pi,
- un convertitore digitale/analogico (DAC) per trasformare un'uscita USB in un'uscita jack audio,
- altoparlanti per computer, che hanno il vantaggio di contenere sia gli altoparlanti sia la parte di amplificazione del segnale,
- un interruttore generale di accensione,
- un doppio cavo USB maschio-femmina da incasso,
- un contenitore in cui sistemare tutto,
- e naturalmente una master keyboard per suonare.

Per configurare facilmente il Raspberry Pi, è consigliabile avere anche:

- uno schermo con un cavo micro-HDMI per il collegamento al Raspberry Pi,
- una tastiera,
- un mouse.

Nell'esempio di questa pagina sono stati usati altoparlanti Edifier R1280T di seconda mano. A un prezzo modesto riproducono i bassi relativamente bene e, come vedrai più avanti, sarà possibile correggere i difetti della risposta in frequenza.

La master keyboard usata è un'Akai MPK Mini Plus, collegabile via USB. L'adattatore USB / jack è un UGREEN di fascia base e il Raspberry Pi è la versione 5.

(lo scopo di questi dettagli non è fare pubblicità a una marca in particolare, ma presentare una combinazione che funziona)

## Preparare il contenitore

Questo tutorial non si concentra su un modo particolare di realizzare il contenitore. Il legno metterà in risalto le tue doti di falegname, come in questo esempio, ma anche il cartone può andare benissimo. Sta a te esprimere la tua creatività! In generale ti servirà un coperchio per accedere ai componenti e delle aperture per:

- gli altoparlanti,
- il cavo di alimentazione,
- l'interruttore di accensione,
- le manopole del volume e, se necessario, dell'equalizzazione,
- i risonatori di Helmholtz del bass-reflex, ecc.

Un possibile ordine di montaggio nel contenitore è il seguente:

- inizia incassando il pulsante di accensione, i connettori USB sul pannello posteriore e gli eventuali altri pulsanti collegati agli altoparlanti,
- posiziona il cavo di alimentazione e collegalo all'ingresso del pulsante di accensione,
- posiziona il trasformatore associato agli altoparlanti e collegalo all'uscita del pulsante di accensione,
- posiziona la scheda elettronica degli altoparlanti e collegala al trasformatore,
- fissa gli altoparlanti e collegali alla scheda elettronica,
- fissa il Raspberry Pi,
- collega l'alimentatore del Raspberry Pi all'uscita del pulsante di accensione (l'uscita del pulsante alimenta sia gli altoparlanti sia il Raspberry Pi),
- collega il doppio cavo USB al Raspberry Pi,
- collega il convertitore USB/jack a un'altra presa USB,
- collega la scheda elettronica degli altoparlanti tramite un cavo jack.

Nell'immagine seguente, l'alimentatore e la scheda elettronica relativi agli altoparlanti sono cerchiati in arancione. Il Raspberry Pi è cerchiato in blu.

![L'interno del sintetizzatore](images/synth_inside.jpg "L'interno del sintetizzatore")

Nota: gli altoparlanti devono essere posizionati in modo che il suono non possa tornare dall'uscita degli altoparlanti verso l'interno del contenitore, altrimenti si crea una risonanza. **Quindi non come in questo esempio!** A volte bisogna sperimentare per scoprire le insidie…

![Il Raspberry Pi](images/synth_raspberry_pi.jpg "Il Raspberry Pi")

Vengono usate 3 porte USB:

- una per ottenere un'uscita jack,
- le altre 2 vengono portate sul retro del contenitore e serviranno a collegare la master keyboard oppure, durante la configurazione del Raspberry Pi, una tastiera e un mouse.

## Preparare il Raspberry Pi

### Installare Debian

Uno dei primi passi da compiere quando si riceve un Raspberry Pi è installare **Debian** su una scheda micro SD. Le istruzioni si trovano facilmente su internet, ad esempio <a href="https://www.raspberrypi.com/documentation/computers/getting-started.html#installing-the-operating-system" target="_blank">qui</a>.

Una volta preparata la scheda micro SD, puoi inserirla nell'apposito alloggiamento del Raspberry Pi. Poi collega l'alimentatore, lo schermo, il mouse e la tastiera, e il tuo computer è pronto!

### Installare e configurare Polyphone

Per prima cosa, aggiorna il sistema con:

```
sudo apt-get update
sudo apt-get upgrade
```

Poi installa **Polyphone** usando il pacchetto .deb disponibile in questa [pagina](software). Collega la tastiera MIDI al Raspberry Pi via USB (scollega la tastiera del computer se non ci sono porte USB libere), poi apri Polyphone e seleziona la tastiera MIDI nell'elenco a discesa degli ingressi MIDI nelle [impostazioni](manual/settings.md#doc_general). Scaricando un soundfont, ad esempio dall'[archivio](soundfonts), è già possibile suonare uno strumento con la tastiera MIDI collegata al Raspberry Pi!

### Avviare Polyphone automaticamente

Questo passaggio serve a non dover più usare lo schermo. Polyphone può essere avviato automaticamente all'accensione del Raspberry Pi creando un file :file:`synthesizer.desktop` con questo contenuto:

```
[Desktop Entry]
Name=Synthesizer
Exec=polyphone -s /home/pi/Desktop/my-soundfont.sf2
Terminal=false
Type=Application
StartupNotify=true
Icon=/home/pi/Desktop/my-icon.png
Categories=AudioVideo;
```

Questo file va poi messo nella cartella :file:`/home/pi/.config/autostart`.

La riga di comando può essere completata con opzioni per ascoltare un canale MIDI specifico, suonare più strumenti contemporaneamente o attivare / disattivare gli strumenti tramite i tasti. Questa [pagina](tutorials/use-polyphone-as-a-synthesizer.md) descrive in dettaglio le opzioni per usare Polyphone in modalità sintetizzatore.

## Per andare oltre

### Equalizzazione del suono

Ogni altoparlante, per la sua conformazione, risponde in modo più o meno diverso a seconda delle frequenze che deve emettere. Per ottenere una risposta in frequenza piatta e riprodurre così fedelmente il suono, è possibile creare un'equalizzazione che compensi i difetti dell'altoparlante. Il programma <a href="https://github.com/erdewit/HiFiScan" target="_blank">HiFiScan</a> calcola automaticamente i parametri dell'equalizzazione; la procedura è descritta nella pagina del progetto.

Una volta calcolata la curva di equalizzazione, **HiFiScan** può esportare i coefficienti da integrare negli equalizzatori. Qui ci interessa l'equalizzazione a 12 bande, utilizzabile con **Calf**.

### Connessione remota

Finora la configurazione del Raspberry Pi ha richiesto uno schermo per visualizzare il desktop. È però possibile configurare il sistema per consentire l'accesso remoto tramite il **protocollo VNC**, e su internet sono disponibili molte risorse per farlo.
