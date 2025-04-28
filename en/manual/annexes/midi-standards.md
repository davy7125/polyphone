## MIDI standards and soundfont compatibility

MIDI standards define rules for the exchange of musical data between electronic devices (keyboards, sound modules, &hellip;) and software (virtual instruments, sequencers, &hellip;) to ensure their compatibility. Regarding soundfonts, these standards specify a series of instruments along with the bank and preset numbers to be used for each. Several standards exist:

* the most well-known is the [General MIDI](#doc_gm) standard, introduced in 1991,
* Roland defined the [MT-32](#doc_mt32) (1987) and [GS](#doc_gs) (1991, 1994, and 1996) standards,
* Yamaha introduced the [XG](#doc_xg) standard in 1994.


## The General MIDI (GM) standard {#doc_gm}


The General MIDI standard is detailed in this [document](files/RP-003_General_MIDI_System_Level_1_Specification_96-1-4_0.1.pdf). Instruments with preset number from 0 to 127 are described below (bank number is usually 0):

<div class='table-columns'>
<table cellspacing="0" cellpadding="1">
<tr><td>000</td><td>Acoustic Grand Piano</td></tr>
<tr><td>001</td><td>Bright Acoustic Piano</td></tr>
<tr><td>002</td><td>Electric Grand Piano</td></tr>
<tr><td>003</td><td>Honky-tonk Piano</td></tr>
<tr><td>004</td><td>Electric Piano 1</td></tr>
<tr><td>005</td><td>Electric Piano 2</td></tr>
<tr><td>006</td><td>Harpsichord</td></tr>
<tr><td>007</td><td>Clavinet</td></tr>
<tr><td>008</td><td>Celesta</td></tr>
<tr><td>009</td><td>Glockenspiel</td></tr>
<tr><td>010</td><td>Music Box</td></tr>
<tr><td>011</td><td>Vibraphone</td></tr>
<tr><td>012</td><td>Marimba</td></tr>
<tr><td>013</td><td>Xylophone</td></tr>
<tr><td>014</td><td>Tubular bells</td></tr>
<tr><td>015</td><td>Dulcimer</td></tr>
<tr><td>016</td><td>Drawbar Organ</td></tr>
<tr><td>017</td><td>Percussive Organ</td></tr>
<tr><td>018</td><td>Rock Organ</td></tr>
<tr><td>019</td><td>Church Organ</td></tr>
<tr><td>020</td><td>Reed Organ</td></tr>
<tr><td>021</td><td>Accordion</td></tr>
<tr><td>022</td><td>Harmonica</td></tr>
<tr><td>023</td><td>Tango Accordion</td></tr>
<tr><td>024</td><td>Acoustic Guitar (nylon)</td></tr>
<tr><td>025</td><td>Acoustic Guitar (steel)</td></tr>
<tr><td>026</td><td>Electric Guitar (jazz)</td></tr>
<tr><td>027</td><td>Electric Guitar (clean)</td></tr>
<tr><td>028</td><td>Electric Guitar (muted)</td></tr>
<tr><td>029</td><td>Overdriven Guitar</td></tr>
<tr><td>030</td><td>Distortion Guitar</td></tr>
<tr><td>031</td><td>Guitar Harmonics</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>032</td><td>Acoustic Bass</td></tr>
<tr><td>033</td><td>Electric Bass (finder)</td></tr>
<tr><td>034</td><td>Electric Bass (pick)</td></tr>
<tr><td>035</td><td>Fretless Bass</td></tr>
<tr><td>036</td><td>Slap Bass 1</td></tr>
<tr><td>037</td><td>Slap Bass 2</td></tr>
<tr><td>038</td><td>Synth Bass 1</td></tr>
<tr><td>039</td><td>Synth Bass 2</td></tr>
<tr><td>040</td><td>Violin</td></tr>
<tr><td>041</td><td>Viola</td></tr>
<tr><td>042</td><td>Cello</td></tr>
<tr><td>043</td><td>Contrabass</td></tr>
<tr><td>044</td><td>Tremolo Strings</td></tr>
<tr><td>045</td><td>Pizzicato Strings</td></tr>
<tr><td>046</td><td>Orchestral Harp</td></tr>
<tr><td>047</td><td>Timpani</td></tr>
<tr><td>048</td><td>Strings Ensemble 1</td></tr>
<tr><td>049</td><td>Strings Ensemble 2 (slow)</td></tr>
<tr><td>050</td><td>Synth Strings 1</td></tr>
<tr><td>051</td><td>Synth Strings 2</td></tr>
<tr><td>052</td><td>Choir Aahs</td></tr>
<tr><td>053</td><td>Voice Oohs</td></tr>
<tr><td>054</td><td>Synth voice</td></tr>
<tr><td>055</td><td>Orchestra Hit</td></tr>
<tr><td>056</td><td>Trumpet</td></tr>
<tr><td>057</td><td>Trombone</td></tr>
<tr><td>058</td><td>Tuba</td></tr>
<tr><td>059</td><td>Muted Trumpet</td></tr>
<tr><td>060</td><td>French Horn</td></tr>
<tr><td>061</td><td>Brass Section</td></tr>
<tr><td>062</td><td>Synth Brass 1</td></tr>
<tr><td>063</td><td>Synth Brass 2</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>064</td><td>Soprano Sax</td></tr>
<tr><td>065</td><td>Alto Sax</td></tr>
<tr><td>066</td><td>Tenor Sax</td></tr>
<tr><td>067</td><td>Baritone Sax</td></tr>
<tr><td>068</td><td>Oboe</td></tr>
<tr><td>069</td><td>English Horn</td></tr>
<tr><td>070</td><td>Bassoon</td></tr>
<tr><td>071</td><td>Clarinet</td></tr>
<tr><td>072</td><td>Piccolo</td></tr>
<tr><td>073</td><td>Flute</td></tr>
<tr><td>074</td><td>Recorder</td></tr>
<tr><td>075</td><td>Pan Flute</td></tr>
<tr><td>076</td><td>Blown Bottle</td></tr>
<tr><td>077</td><td>Shakuhachi</td></tr>
<tr><td>078</td><td>Whistle</td></tr>
<tr><td>079</td><td>Ocarina</td></tr>
<tr><td>080</td><td>Lead 1 (square)</td></tr>
<tr><td>081</td><td>Lead 2 (sawtooth)</td></tr>
<tr><td>082</td><td>Lead 3 (calliope)</td></tr>
<tr><td>083</td><td>Lead 4 (chiff)</td></tr>
<tr><td>084</td><td>Lead 5 (charang)</td></tr>
<tr><td>085</td><td>Lead 6 (voice)</td></tr>
<tr><td>086</td><td>Lead 7 (fifths)</td></tr>
<tr><td>087</td><td>Lead 8 (bass + lead)</td></tr>
<tr><td>088</td><td>Pad 1 (new age)</td></tr>
<tr><td>089</td><td>Pad 2 (warm)</td></tr>
<tr><td>090</td><td>Pad 3 (polysynth)</td></tr>
<tr><td>091</td><td>Pad 4 (choir)</td></tr>
<tr><td>092</td><td>Pad 5 (bowed)</td></tr>
<tr><td>093</td><td>Pad 6 (metallic)</td></tr>
<tr><td>094</td><td>Pad 7 (halo)</td></tr>
<tr><td>095</td><td>Pad 8 (sweep)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>096</td><td>FX 1 (rain)</td></tr>
<tr><td>097</td><td>FX 2 (soundtrack)</td></tr>
<tr><td>098</td><td>FX 3 (crystal)</td></tr>
<tr><td>099</td><td>FX 4 (atmosphere)</td></tr>
<tr><td>100</td><td>FX 5 (brightness)</td></tr>
<tr><td>101</td><td>FX 6 (goblins)</td></tr>
<tr><td>102</td><td>FX 7 (echoes)</td></tr>
<tr><td>103</td><td>FX 8 (sci-fi)</td></tr>
<tr><td>104</td><td>Sitar</td></tr>
<tr><td>105</td><td>Banjo</td></tr>
<tr><td>106</td><td>Shamisen</td></tr>
<tr><td>107</td><td>Koto</td></tr>
<tr><td>108</td><td>Kalimba</td></tr>
<tr><td>109</td><td>Bag pipe</td></tr>
<tr><td>110</td><td>Fiddle</td></tr>
<tr><td>111</td><td>Shanai</td></tr>
<tr><td>112</td><td>Tinkle Bell</td></tr>
<tr><td>113</td><td>Agogo</td></tr>
<tr><td>114</td><td>Steel Drums</td></tr>
<tr><td>115</td><td>Woodblock</td></tr>
<tr><td>116</td><td>Taiko Drum</td></tr>
<tr><td>117</td><td>Melodic Tom</td></tr>
<tr><td>118</td><td>Synth Drum</td></tr>
<tr><td>119</td><td>Reverse Cymbal</td></tr>
<tr><td>120</td><td>Guitar Fret Noise</td></tr>
<tr><td>121</td><td>Breath Noise</td></tr>
<tr><td>122</td><td>Seashore</td></tr>
<tr><td>123</td><td>Bird Tweet</td></tr>
<tr><td>124</td><td>Telephone Ring</td></tr>
<tr><td>125</td><td>Helicopter</td></tr>
<tr><td>126</td><td>Applause</td></tr>
<tr><td>127</td><td>Gun Shot</td></tr>
</table>
</div>

Percussion instruments, called by preset 0 of bank 128 for example, are distributed as follows:

<div class='table-columns'>
<table cellspacing="0" cellpadding="1">
<tr><td>035</td><td>Acoustic Bass Drum</td></tr>
<tr><td>036</td><td>Bass Drum 1</td></tr>
<tr><td>037</td><td>Side Stick </td></tr>
<tr><td>038</td><td>Acoustic Snare </td></tr>
<tr><td>039</td><td>Hand Clap</td></tr>
<tr><td>040</td><td>Electric Snare</td></tr>
<tr><td>041</td><td>Low Floor Tom</td></tr>
<tr><td>042</td><td>Closed Hi Hat</td></tr>
<tr><td>043</td><td>High Floor Tom</td></tr>
<tr><td>044</td><td>Pedal Hi-Hat</td></tr>
<tr><td>045</td><td>Low Tom</td></tr>
<tr><td>046</td><td>Open Hi-Hat</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>047</td><td>Low-Mid Tom</td></tr>
<tr><td>048</td><td>Hi Mid Tom</td></tr>
<tr><td>049</td><td>Crash Cymbal 1</td></tr>
<tr><td>050</td><td>High Tom</td></tr>
<tr><td>051</td><td>Ride Cymbal 1</td></tr>
<tr><td>052</td><td>Chinese Cymbal</td></tr>
<tr><td>053</td><td>Ride Bell </td></tr>
<tr><td>054</td><td>Tambourine</td></tr>
<tr><td>055</td><td>Splash Cymbal</td></tr>
<tr><td>056</td><td>Cowbell</td></tr>
<tr><td>057</td><td>Crash Cymbal 2</td></tr>
<tr><td>058</td><td>Vibraslap</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>059</td><td>Ride Cymbal 2</td></tr>
<tr><td>060</td><td>Hi Bongo</td></tr>
<tr><td>061</td><td>Low Bongo</td></tr>
<tr><td>062</td><td>Mute Hi Conga</td></tr>
<tr><td>063</td><td>Open Hi Conga</td></tr>
<tr><td>064</td><td>Low Conga</td></tr>
<tr><td>065</td><td>High Timbale</td></tr>
<tr><td>066</td><td>Low Timbale</td></tr>
<tr><td>067</td><td>High Agogo</td></tr>
<tr><td>068</td><td>Low Agogo</td></tr>
<tr><td>069</td><td>Cabasa</td></tr>
<tr><td>070</td><td>Maracas</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>071</td><td>Short Whistle</td></tr>
<tr><td>072</td><td>Long Whistle</td></tr>
<tr><td>073</td><td>Short Guiro</td></tr>
<tr><td>074</td><td>Long Guiro</td></tr>
<tr><td>075</td><td>Claves</td></tr>
<tr><td>076</td><td>Hi Wood Block</td></tr>
<tr><td>077</td><td>Low Wood Block</td></tr>
<tr><td>078</td><td>Mute Cuica</td></tr>
<tr><td>079</td><td>Open Cuica</td></tr>
<tr><td>080</td><td>Mute Triangle</td></tr>
<tr><td>081</td><td>Open Triangle</td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
</table>
</div>


## The Roland MT-32 standard {#doc_mt32}


The MT-32 standard is detailed in this [document](files/Roland_MT-32_OM.pdf). The bank number is not important but the instruments, according to the preset number, are the following:

<div class='table-columns'>
<table cellspacing="0" cellpadding="1">
<tr><td>000</td><td>Acoustic Piano 1</td></tr>
<tr><td>001</td><td>Acoustic Piano 2</td></tr>
<tr><td>002</td><td>Acoustic Piano 3</td></tr>
<tr><td>003</td><td>Electronic Piano 1</td></tr>
<tr><td>004</td><td>Electronic Piano 2</td></tr>
<tr><td>005</td><td>Electronic Piano 3</td></tr>
<tr><td>006</td><td>Electronic Piano 4</td></tr>
<tr><td>007</td><td>Honkytonk</td></tr>
<tr><td>008</td><td>Electronic Organ 1</td></tr>
<tr><td>009</td><td>Electronic Organ 2</td></tr>
<tr><td>010</td><td>Electronic Organ 3</td></tr>
<tr><td>011</td><td>Electronic Organ 4</td></tr>
<tr><td>012</td><td>Pipe Organ 1</td></tr>
<tr><td>013</td><td>Pipe Organ 2</td></tr>
<tr><td>014</td><td>Pipe Organ 3</td></tr>
<tr><td>015</td><td>Accordion</td></tr>
<tr><td>016</td><td>Harpsichord 1</td></tr>
<tr><td>017</td><td>Harpsichord 2</td></tr>
<tr><td>018</td><td>Harpsichord 3</td></tr>
<tr><td>019</td><td>Clavinet 1</td></tr>
<tr><td>020</td><td>Clavinet 2</td></tr>
<tr><td>021</td><td>Clavinet 3</td></tr>
<tr><td>022</td><td>Celesta 1</td></tr>
<tr><td>023</td><td>Celesta 2</td></tr>
<tr><td>024</td><td>Synth Brass 1</td></tr>
<tr><td>025</td><td>Synth Brass 2</td></tr>
<tr><td>026</td><td>Synth Brass 3</td></tr>
<tr><td>027</td><td>Synth Brass 4</td></tr>
<tr><td>028</td><td>Synth Bass 1</td></tr>
<tr><td>029</td><td>Synth Bass 2</td></tr>
<tr><td>030</td><td>Synth Bass 3</td></tr>
<tr><td>031</td><td>Synth Bass 4</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>032</td><td>Fantasy</td></tr>
<tr><td>033</td><td>Harmo Pan</td></tr>
<tr><td>034</td><td>Chorale</td></tr>
<tr><td>035</td><td>Glasses</td></tr>
<tr><td>036</td><td>Soundtrack</td></tr>
<tr><td>037</td><td>Atmosphere</td></tr>
<tr><td>038</td><td>Warm Bell</td></tr>
<tr><td>039</td><td>Funny Vox</td></tr>
<tr><td>040</td><td>Echo Bell</td></tr>
<tr><td>041</td><td>Ice Rain</td></tr>
<tr><td>042</td><td>Oboe 2001</td></tr>
<tr><td>043</td><td>Echo Pan</td></tr>
<tr><td>044</td><td>Doctor Solo</td></tr>
<tr><td>045</td><td>School Daze</td></tr>
<tr><td>046</td><td>Bellsinger</td></tr>
<tr><td>047</td><td>Square Wave</td></tr>
<tr><td>048</td><td>String Section 1</td></tr>
<tr><td>049</td><td>String Section 2</td></tr>
<tr><td>050</td><td>String Section 3</td></tr>
<tr><td>051</td><td>Pizzicato</td></tr>
<tr><td>052</td><td>Violin 1</td></tr>
<tr><td>053</td><td>Violin 2</td></tr>
<tr><td>054</td><td>Cello 1</td></tr>
<tr><td>055</td><td>Cello 2</td></tr>
<tr><td>056</td><td>Contrabass</td></tr>
<tr><td>057</td><td>Harp 1</td></tr>
<tr><td>058</td><td>Harp 2</td></tr>
<tr><td>059</td><td>Guitar 1</td></tr>
<tr><td>060</td><td>Guitar 2</td></tr>
<tr><td>061</td><td>Electric Gtr 1</td></tr>
<tr><td>062</td><td>Electric Gtr 2</td></tr>
<tr><td>063</td><td>Sitar</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>064</td><td>Acoustic Bass 1</td></tr>
<tr><td>065</td><td>Acoustic Bass 2</td></tr>
<tr><td>066</td><td>Electric Bass 1</td></tr>
<tr><td>067</td><td>Electric Bass 2</td></tr>
<tr><td>068</td><td>Slap Bass 1</td></tr>
<tr><td>069</td><td>Slap Bass 2</td></tr>
<tr><td>070</td><td>Fretless 1</td></tr>
<tr><td>071</td><td>Fretless 2</td></tr>
<tr><td>072</td><td>Flute 1</td></tr>
<tr><td>073</td><td>Flute 2</td></tr>
<tr><td>074</td><td>Piccolo 1</td></tr>
<tr><td>075</td><td>Piccolo 2</td></tr>
<tr><td>076</td><td>Recorder</td></tr>
<tr><td>077</td><td>Pan Pipes</td></tr>
<tr><td>078</td><td>Sax 1</td></tr>
<tr><td>079</td><td>Sax 2</td></tr>
<tr><td>080</td><td>Sax 3</td></tr>
<tr><td>081</td><td>Sax 4</td></tr>
<tr><td>082</td><td>Clarinet 1</td></tr>
<tr><td>083</td><td>Clarinet 2</td></tr>
<tr><td>084</td><td>Oboe</td></tr>
<tr><td>085</td><td>English Horn</td></tr>
<tr><td>086</td><td>Bassoon</td></tr>
<tr><td>087</td><td>Harmonica</td></tr>
<tr><td>088</td><td>Trumpet 1</td></tr>
<tr><td>089</td><td>Trumpet 2</td></tr>
<tr><td>090</td><td>Trombone 1</td></tr>
<tr><td>091</td><td>Trombone 2</td></tr>
<tr><td>092</td><td>French Horn 1</td></tr>
<tr><td>093</td><td>French Horn 2</td></tr>
<tr><td>094</td><td>Tuba</td></tr>
<tr><td>095</td><td>Brass Section 1</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>096</td><td>Brass Section 2</td></tr>
<tr><td>097</td><td>Vibe 1</td></tr>
<tr><td>098</td><td>Vibe 2</td></tr>
<tr><td>099</td><td>Synth Mallet</td></tr>
<tr><td>100</td><td>Windbell</td></tr>
<tr><td>101</td><td>Glock</td></tr>
<tr><td>102</td><td>Tube Bell</td></tr>
<tr><td>103</td><td>Xylophone</td></tr>
<tr><td>104</td><td>Marimba</td></tr>
<tr><td>105</td><td>Koto</td></tr>
<tr><td>106</td><td>Sho</td></tr>
<tr><td>107</td><td>Shakuhachi</td></tr>
<tr><td>108</td><td>Whistle 1</td></tr>
<tr><td>109</td><td>Whistle 2</td></tr>
<tr><td>110</td><td>Bottleblow</td></tr>
<tr><td>111</td><td>Breathpipe</td></tr>
<tr><td>112</td><td>Timpani</td></tr>
<tr><td>113</td><td>Melodic Tom</td></tr>
<tr><td>114</td><td>Deep Snare</td></tr>
<tr><td>115</td><td>Elec Perc 1</td></tr>
<tr><td>116</td><td>Elec Perc 2</td></tr>
<tr><td>117</td><td>Taiko</td></tr>
<tr><td>118</td><td>Taiko Rim</td></tr>
<tr><td>119</td><td>Cymbal</td></tr>
<tr><td>120</td><td>Castanets</td></tr>
<tr><td>121</td><td>Triangle</td></tr>
<tr><td>122</td><td>Orchestral Hit</td></tr>
<tr><td>123</td><td>Telephone</td></tr>
<tr><td>124</td><td>Bird Tweet</td></tr>
<tr><td>125</td><td>One Note Jam</td></tr>
<tr><td>126</td><td>Water Bell</td></tr>
<tr><td>127</td><td>Jungle Tune</td></tr>
</table>
</div>


## The Roland GS standard {#doc_gs}


The GS standard was extended twice. The initial 1991 version is detailed in this [document](files/Roland_SC-55_OM.pdf), the 1994 update in [this&nbsp;one](files/Roland_SC-88_OM.pdf), and the 1996 update in [this&nbsp;one](files/Roland_SC-88PRO_OM.pdf). The instrument list includes all [General MIDI](#doc_gm) standard instruments and all the following items:

<div class='table-columns'>
<table cellspacing="0" cellpadding="1">
<tr><td>008:000</td><td>Piano 1 (1 w)</td></tr>
<tr><td>016:000</td><td>Piano 1 (1 d)</td></tr>
<tr><td>024:000</td><td>Piano + String (3)</td></tr>
<tr><td>008:001</td><td>Piano 2 (1 w)</td></tr>
<tr><td>016:001</td><td>Dance Piano (3)</td></tr>
<tr><td>001:002</td><td>EG + Rhodes 1 (2)</td></tr>
<tr><td>002:002</td><td>EG + Rhodes 2 (2)</td></tr>
<tr><td>008:002</td><td>Piano 3 (1 w)</td></tr>
<tr><td>008:003</td><td>Honky-tonk (1 w)</td></tr>
<tr><td>008:004</td><td>Detuned EP 1 (1)</td></tr>
<tr><td>009:004</td><td>Chorused E. Piano (3)</td></tr>
<tr><td>010:004</td><td>Silent Rhodes (3)</td></tr>
<tr><td>016:004</td><td>E. Piano 1 (1 w)</td></tr>
<tr><td>017:004</td><td>Dist E. Piano (3)</td></tr>
<tr><td>024:004</td><td>60's E. Piano (1)</td></tr>
<tr><td>025:004</td><td>Hard Rhodes (2)</td></tr>
<tr><td>026:004</td><td>Mellow Rhodes (2)</td></tr>
<tr><td>008:005</td><td>Detuned EP 2 (1)</td></tr>
<tr><td>016:005</td><td>E. Piano 2 (1 w)</td></tr>
<tr><td>024:005</td><td>Hard FM EP (2)</td></tr>
<tr><td>001:006</td><td>Harpsichord 2 (3)</td></tr>
<tr><td>008:006</td><td>Coupled Harpsichord (1)</td></tr>
<tr><td>016:006</td><td>Harpsichord (1 w)</td></tr>
<tr><td>024:006</td><td>Harpsichord (1 o)</td></tr>
<tr><td>032:006</td><td>Synth Harpsichord (3)</td></tr>
<tr><td>008:007</td><td>Comp Clavinet (3)</td></tr>
<tr><td>016:007</td><td>Reso Clavinet (3)</td></tr>
<tr><td>024:007</td><td>Clavinet (3 o)</td></tr>
<tr><td>032:007</td><td>Analog Clavinet (3)</td></tr>
<tr><td>033:007</td><td>JP8 Clavinet 1 (3)</td></tr>
<tr><td>035:007</td><td>JP8 Clavinet 2 (3)</td></tr>
<tr><td>001:008</td><td>Pop Celesta (3)</td></tr>
<tr><td>001:011</td><td>Hard Vibe (2)</td></tr>
<tr><td>008:011</td><td>Vibraphone (1 w)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>009:011</td><td>Vibraphones (3)</td></tr>
<tr><td>008:012</td><td>Marimba (1 w)</td></tr>
<tr><td>016:012</td><td>Barafon (2)</td></tr>
<tr><td>017:012</td><td>Barafon 2 (2)</td></tr>
<tr><td>024:012</td><td>Log Drum (2)</td></tr>
<tr><td>008:014</td><td>Church Bell (1)</td></tr>
<tr><td>009:014</td><td>Carillon (1)</td></tr>
<tr><td>001:015</td><td>Santur 2 (2)</td></tr>
<tr><td>008:015</td><td>Cimbalom (2)</td></tr>
<tr><td>016:015</td><td>Zither 1 (3)</td></tr>
<tr><td>017:015</td><td>Zither 2 (3)</td></tr>
<tr><td>024:015</td><td>Dulcimer (3)</td></tr>
<tr><td>001:016</td><td>Organ 101 (2)</td></tr>
<tr><td>008:016</td><td>Detuned Organ 1 (1)</td></tr>
<tr><td>009:016</td><td>Organ 109 (2)</td></tr>
<tr><td>016:016</td><td>60's Organ 1 (1)</td></tr>
<tr><td>017:016</td><td>60's Organ 2 (2)</td></tr>
<tr><td>018:016</td><td>60's Organ 3 (2)</td></tr>
<tr><td>019:016</td><td>Farf Organ (3)</td></tr>
<tr><td>024:016</td><td>Cheese Organ (2)</td></tr>
<tr><td>025:016</td><td>D-50 Organ (3)</td></tr>
<tr><td>026:016</td><td>JUNO Organ (3)</td></tr>
<tr><td>027:016</td><td>Hybrid Organ (3)</td></tr>
<tr><td>028:016</td><td>VS Organ (3)</td></tr>
<tr><td>029:016</td><td>Digi Church (3)</td></tr>
<tr><td>032:016</td><td>Organ 4 (1)</td></tr>
<tr><td>033:016</td><td>Even Bar (2)</td></tr>
<tr><td>040:016</td><td>Organ Bass (2)</td></tr>
<tr><td>048:016</td><td>5th Organ (3)</td></tr>
<tr><td>001:017</td><td>Organ 201 (2)</td></tr>
<tr><td>002:017</td><td>E. Organ 16+2 (3)</td></tr>
<tr><td>008:017</td><td>Detuned Organ 2 (1)</td></tr>
<tr><td>009:017</td><td>Octave Organ (3)</td></tr>
<tr><td>032:017</td><td>Organ 5 (1)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>008:018</td><td>Rotary Organ (2)</td></tr>
<tr><td>016:018</td><td>Rotary Organ S (2)</td></tr>
<tr><td>017:018</td><td>Rock Organ 1 (3)</td></tr>
<tr><td>018:018</td><td>Rock Organ 2 (3)</td></tr>
<tr><td>024:018</td><td>Rotary Organ F (2)</td></tr>
<tr><td>008:019</td><td>Church Organ 2 (1)</td></tr>
<tr><td>016:019</td><td>Church Organ 3 (1)</td></tr>
<tr><td>024:019</td><td>Organ Flute (2)</td></tr>
<tr><td>032:019</td><td>Tremolo Flute (2)</td></tr>
<tr><td>033:019</td><td>Theater Org. (3)</td></tr>
<tr><td>008:020</td><td>Wind Organ (3)</td></tr>
<tr><td>008:021</td><td>Accordion Italian (1)</td></tr>
<tr><td>009:021</td><td>Distorted Accord (3)</td></tr>
<tr><td>016:021</td><td>Chorused Accord (3)</td></tr>
<tr><td>024:021</td><td>Hard Accord (3)</td></tr>
<tr><td>025:021</td><td>Soft Accord (3)</td></tr>
<tr><td>001:022</td><td>Harmonica 2 (2)</td></tr>
<tr><td>008:023</td><td>Bandoneon 2 (3)</td></tr>
<tr><td>016:023</td><td>Bandoneon 3 (3)</td></tr>
<tr><td>008:024</td><td>Ukulele (1)</td></tr>
<tr><td>016:024</td><td>Nylon Guitar (1 o)</td></tr>
<tr><td>024:024</td><td>Velo Harmonics (2)</td></tr>
<tr><td>032:024</td><td>Nylon Guitar 2 (1)</td></tr>
<tr><td>040:024</td><td>Lequint Guitar. (2)</td></tr>
<tr><td>008:025</td><td>12 String Guitar (1)</td></tr>
<tr><td>009:025</td><td>Nylon + Steel (2)</td></tr>
<tr><td>016:025</td><td>Mandolin (1)</td></tr>
<tr><td>017:025</td><td>Mandolin 2 (3)</td></tr>
<tr><td>018:025</td><td>Mandolin 3 (3)</td></tr>
<tr><td>032:025</td><td>Steel Guitar 2 (2)</td></tr>
<tr><td>001:026</td><td>Mellow Guitar (2)</td></tr>
<tr><td>008:026</td><td>Hawaiian Guitar (1)</td></tr>
<tr><td>001:027</td><td>Clean Half (3)</td></tr>
<tr><td>002:027</td><td>Open Hard 1 (3)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>003:027</td><td>Open Hard 2 (3)</td></tr>
<tr><td>004:027</td><td>JC Clean Guitar. (3)</td></tr>
<tr><td>008:027</td><td>Chorus Guitar (1)</td></tr>
<tr><td>009:027</td><td>JC Chorus Guitar (3)</td></tr>
<tr><td>016:027</td><td>TC Front Pick (3)</td></tr>
<tr><td>017:027</td><td>TC Rear Pick (3)</td></tr>
<tr><td>018:027</td><td>TC Clean ff (3)</td></tr>
<tr><td>019:027</td><td>TC Clean 2 (3 l)</td></tr>
<tr><td>001:028</td><td>Muted Distorted Guitar (2)</td></tr>
<tr><td>002:028</td><td>TC Muted Guitar (3)</td></tr>
<tr><td>008:028</td><td>Funk Guitar (1)</td></tr>
<tr><td>016:028</td><td>Funk Guitar 2 (1)</td></tr>
<tr><td>001:029</td><td>Overdrive 2 (3)</td></tr>
<tr><td>002:029</td><td>Overdrive 3 (3)</td></tr>
<tr><td>003:029</td><td>More Drive (3)</td></tr>
<tr><td>008:029</td><td>LP Over Drive Guitar (3)</td></tr>
<tr><td>009:029</td><td>LP Over Drive (3 l)</td></tr>
<tr><td>001:030</td><td>Distortion Guitar 2 (2)</td></tr>
<tr><td>002:030</td><td>Dazed Guitar (2)</td></tr>
<tr><td>003:030</td><td>Distortion (3 l)</td></tr>
<tr><td>004:030</td><td>Distortion Fast (3 l)</td></tr>
<tr><td>008:030</td><td>Feedback Guitar (1)</td></tr>
<tr><td>009:030</td><td>Feedback Guitar 2 (2)</td></tr>
<tr><td>016:030</td><td>Power Guitar (2)</td></tr>
<tr><td>017:030</td><td>Power Guitar 2 (2)</td></tr>
<tr><td>018:030</td><td>5th Distortion Chord (2)</td></tr>
<tr><td>024:030</td><td>Rock Rhythm (2)</td></tr>
<tr><td>025:030</td><td>Rock Rhythm 2 (2)</td></tr>
<tr><td>008:031</td><td>Guitar Feedback (1)</td></tr>
<tr><td>009:031</td><td>Guitar Feedback 2 (3)</td></tr>
<tr><td>016:031</td><td>Acoustic Guitar Harmonics (2)</td></tr>
<tr><td>024:031</td><td>Electric Bass Harmonics (3)</td></tr>
<tr><td>001:032</td><td>Rockabilly (3)</td></tr>
<tr><td>008:032</td><td>Wild Acoustic Bass (3)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>016:032</td><td>Bass + OHH (3)</td></tr>
<tr><td>001:033</td><td>Fingered Bass 2 (2)</td></tr>
<tr><td>002:033</td><td>Jazz Bass (2)</td></tr>
<tr><td>003:033</td><td>Jazz Bass 2 (3)</td></tr>
<tr><td>004:033</td><td>Rock Bass (3)</td></tr>
<tr><td>008:033</td><td>Chorus Jazz Bass (3)</td></tr>
<tr><td>016:033</td><td>Fingered Bass/Harm (3)</td></tr>
<tr><td>001:034</td><td>Picked Bass 2 (3)</td></tr>
<tr><td>002:034</td><td>Picked Bass 3 (3)</td></tr>
<tr><td>003:034</td><td>Picked Bass 4 (3)</td></tr>
<tr><td>008:034</td><td>Mute Pick Bass (2)</td></tr>
<tr><td>016:034</td><td>Picked Bass/Harm (3)</td></tr>
<tr><td>001:035</td><td>Fretless Bass 2 (2)</td></tr>
<tr><td>002:035</td><td>Fretless Bass 3 (2)</td></tr>
<tr><td>003:035</td><td>Fretless Bass 4 (2)</td></tr>
<tr><td>004:035</td><td>Synth Fretless (2)</td></tr>
<tr><td>005:035</td><td>Mr. Smooth (2)</td></tr>
<tr><td>008:035</td><td>Wood + Fretless Bass (3)</td></tr>
<tr><td>001:036</td><td>Slap Pop (3)</td></tr>
<tr><td>008:036</td><td>Reso Slap (2)</td></tr>
<tr><td>009:036</td><td>Unison Slap (3)</td></tr>
<tr><td>008:037</td><td>FM Slap (3)</td></tr>
<tr><td>001:038</td><td>Synth Bass 101 (1)</td></tr>
<tr><td>002:038</td><td>CS Bass (3)</td></tr>
<tr><td>003:038</td><td>JP-4 Bass (3)</td></tr>
<tr><td>004:038</td><td>JP-8 Bass (3)</td></tr>
<tr><td>005:038</td><td>P5 Bass (3)</td></tr>
<tr><td>006:038</td><td>JPMG Bass (3)</td></tr>
<tr><td>008:038</td><td>Synth Bass 3 (1)</td></tr>
<tr><td>009:038</td><td>TB303 Bass (2)</td></tr>
<tr><td>010:038</td><td>Tekno Bass (2)</td></tr>
<tr><td>011:038</td><td>TB303 Bass 2 (3)</td></tr>
<tr><td>012:038</td><td>Kicked TB303 (3)</td></tr>
<tr><td>013:038</td><td>TB303 Saw Bass (3)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>014:038</td><td>Rubber303 Bass (3)</td></tr>
<tr><td>015:038</td><td>Reso 303 Bass (3)</td></tr>
<tr><td>016:038</td><td>Reso SH Bass (2)</td></tr>
<tr><td>017:038</td><td>TB303 Square Bass (3)</td></tr>
<tr><td>018:038</td><td>TB303 Distorted Bass (3)</td></tr>
<tr><td>024:038</td><td>Arpeggio Bass (3)</td></tr>
<tr><td>001:039</td><td>Synth Bass 201 (2)</td></tr>
<tr><td>002:039</td><td>Modular Bass (2)</td></tr>
<tr><td>003:039</td><td>Sequenced Bass (2)</td></tr>
<tr><td>004:039</td><td>MG Bass (3)</td></tr>
<tr><td>005:039</td><td>MG Octave Bass 1 (3)</td></tr>
<tr><td>006:039</td><td>MG Octave Bass 2 (3)</td></tr>
<tr><td>007:039</td><td>MG Blip Bass (3 l)</td></tr>
<tr><td>008:039</td><td>Synth Bass 4 (1)</td></tr>
<tr><td>009:039</td><td>Dry Bass (2)</td></tr>
<tr><td>010:039</td><td>X Wire Bass (3)</td></tr>
<tr><td>011:039</td><td>Wire String Bass (3)</td></tr>
<tr><td>012:039</td><td>Blip Bass (3 l)</td></tr>
<tr><td>013:039</td><td>Rubber Bass 1 (3)</td></tr>
<tr><td>016:039</td><td>Rubber Bass (1)</td></tr>
<tr><td>017:039</td><td>SH101 Bass 1 (2)</td></tr>
<tr><td>018:039</td><td>SH101 Bass 2 (2)</td></tr>
<tr><td>019:039</td><td>Smooth Bass (2)</td></tr>
<tr><td>020:039</td><td>SH101 Bass 3 (3)</td></tr>
<tr><td>021:039</td><td>Spike Bass (3)</td></tr>
<tr><td>022:039</td><td>House Bass (3 l)</td></tr>
<tr><td>023:039</td><td>KG Bass (3)</td></tr>
<tr><td>024:039</td><td>Sync Bass (3)</td></tr>
<tr><td>025:039</td><td>MG 5th Bass (3)</td></tr>
<tr><td>026:039</td><td>RND Bass (3)</td></tr>
<tr><td>027:039</td><td>Wow MG Bass (3)</td></tr>
<tr><td>028:039</td><td>Bubble Bass (3)</td></tr>
<tr><td>001:040</td><td>Violin Atk (3 l)</td></tr>
<tr><td>008:040</td><td>Slow Violin (1)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>001:041</td><td>Viola Atk (3 l)</td></tr>
<tr><td>001:042</td><td>Cello Atk (3 l)</td></tr>
<tr><td>008:044</td><td>Slow Tremolo (2)</td></tr>
<tr><td>009:044</td><td>Suspense Strings (2)</td></tr>
<tr><td>001:045</td><td>Vcs &amp; Cbs Pizzicato (3)</td></tr>
<tr><td>002:045</td><td>Chamber Pizzicato (3)</td></tr>
<tr><td>003:045</td><td>St. Pizzicato (3)</td></tr>
<tr><td>008:045</td><td>Solo Pizzicato (3)</td></tr>
<tr><td>016:045</td><td>Solo Spic (3)</td></tr>
<tr><td>016:046</td><td>Synth Harp (3)</td></tr>
<tr><td>001:048</td><td>Strings 2 (2)</td></tr>
<tr><td>002:048</td><td>Chamber Strings (3 l)</td></tr>
<tr><td>003:048</td><td>Cello section (3)</td></tr>
<tr><td>008:048</td><td>Orchestra (1)</td></tr>
<tr><td>009:048</td><td>Orchestra 2 (2)</td></tr>
<tr><td>010:048</td><td>Tremolo Orchestra (2)</td></tr>
<tr><td>011:048</td><td>Choir Strings (2)</td></tr>
<tr><td>012:048</td><td>Strings + Horn (3)</td></tr>
<tr><td>016:048</td><td>St. Strings (2)</td></tr>
<tr><td>024:048</td><td>Velo Strings (2)</td></tr>
<tr><td>032:048</td><td>Octave Strings 1 (3)</td></tr>
<tr><td>033:048</td><td>Octave Strings 2 (3)</td></tr>
<tr><td>001:049</td><td>Slow Strings 2 (2)</td></tr>
<tr><td>008:049</td><td>Legato Strings (2)</td></tr>
<tr><td>009:049</td><td>Warm Strings (2)</td></tr>
<tr><td>010:049</td><td>St. Slow Strings (2)</td></tr>
<tr><td>001:050</td><td>OB Strings (2)</td></tr>
<tr><td>002:050</td><td>Stack Strings (3)</td></tr>
<tr><td>003:050</td><td>JP Strings (3)</td></tr>
<tr><td>008:050</td><td>Synth Strings 3 (1)</td></tr>
<tr><td>009:050</td><td>Synth Strings 4 (3)</td></tr>
<tr><td>016:050</td><td>High Strings (3)</td></tr>
<tr><td>017:050</td><td>Hybrid Strings (3)</td></tr>
<tr><td>024:050</td><td>Tron Strings (3)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>025:050</td><td>Noise Strings (3)</td></tr>
<tr><td>001:051</td><td>Synth Strings5 (3)</td></tr>
<tr><td>002:051</td><td>JUNO Strings (3)</td></tr>
<tr><td>008:051</td><td>Air Strings (3)</td></tr>
<tr><td>008:052</td><td>St. Choir (2)</td></tr>
<tr><td>009:052</td><td>Mello Choir (2)</td></tr>
<tr><td>010:052</td><td>Church Choir (3)</td></tr>
<tr><td>016:052</td><td>Choir Hahs (3)</td></tr>
<tr><td>024:052</td><td>Chorus Lahs (3)</td></tr>
<tr><td>032:052</td><td>Choir Aahs 2 (3)</td></tr>
<tr><td>033:052</td><td>Male Aah + String (3)</td></tr>
<tr><td>008:053</td><td>Voice Dahs (3)</td></tr>
<tr><td>008:054</td><td>Synth Voice (2)</td></tr>
<tr><td>009:054</td><td>Silent Night (3)</td></tr>
<tr><td>016:054</td><td>VP330 Choir (3)</td></tr>
<tr><td>017:054</td><td>Vinyl Choir (3)</td></tr>
<tr><td>008:055</td><td>Impact Hit (2)</td></tr>
<tr><td>009:055</td><td>Philly Hit (2)</td></tr>
<tr><td>010:055</td><td>Double Hit (2)</td></tr>
<tr><td>011:055</td><td>Percussive Hit (3)</td></tr>
<tr><td>012:055</td><td>Shock Wave (3)</td></tr>
<tr><td>016:055</td><td>Lo Fi Rave (2)</td></tr>
<tr><td>017:055</td><td>Techno Hit (3)</td></tr>
<tr><td>018:055</td><td>Dist. Hit (3)</td></tr>
<tr><td>019:055</td><td>Bam Hit (3)</td></tr>
<tr><td>020:055</td><td>Bit Hit (3)</td></tr>
<tr><td>021:055</td><td>Bim Hit (3)</td></tr>
<tr><td>022:055</td><td>Technorg Hit (3)</td></tr>
<tr><td>023:055</td><td>Rave Hit (3)</td></tr>
<tr><td>024:055</td><td>Strings Hit (3)</td></tr>
<tr><td>025:055</td><td>Stack Hit (3)</td></tr>
<tr><td>001:056</td><td>Trumpet 2 (2)</td></tr>
<tr><td>002:056</td><td>Trumpet (3 l)</td></tr>
<tr><td>008:056</td><td>Flugel Horn (2)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>016:056</td><td>4th Trumpets (3)</td></tr>
<tr><td>024:056</td><td>Bright Trumpet (2)</td></tr>
<tr><td>025:056</td><td>Warm Trumpet (2)</td></tr>
<tr><td>032:056</td><td>Synth Trumpet (3)</td></tr>
<tr><td>001:057</td><td>Trombone 2 (1)</td></tr>
<tr><td>002:057</td><td>Twin bones (3)</td></tr>
<tr><td>008:057</td><td>Bass Trombone (3)</td></tr>
<tr><td>001:058</td><td>Tuba 2 (2)</td></tr>
<tr><td>008:059</td><td>Muted Horns (3)</td></tr>
<tr><td>001:060</td><td>French Horn2 (1)</td></tr>
<tr><td>002:060</td><td>Horn + Orch (3)</td></tr>
<tr><td>003:060</td><td>Wide French Horns (3)</td></tr>
<tr><td>008:060</td><td>French Horn Solo (2)</td></tr>
<tr><td>009:060</td><td>Dual Horns (3)</td></tr>
<tr><td>016:060</td><td>Horn Orch (2)</td></tr>
<tr><td>024:060</td><td>French Horn Rip (3)</td></tr>
<tr><td>001:061</td><td>Brass ff (3)</td></tr>
<tr><td>002:061</td><td>Bones Section (3)</td></tr>
<tr><td>008:061</td><td>Brass 2 (1)</td></tr>
<tr><td>009:061</td><td>Brass 3 (3)</td></tr>
<tr><td>010:061</td><td>Brass SFZ (3)</td></tr>
<tr><td>016:061</td><td>Brass Fall (2)</td></tr>
<tr><td>017:061</td><td>Trumpet Fall (3)</td></tr>
<tr><td>024:061</td><td>Octave Brass (3)</td></tr>
<tr><td>025:061</td><td>Brass + Reed (3)</td></tr>
<tr><td>001:062</td><td>Poly Brass (2)</td></tr>
<tr><td>002:062</td><td>Stack Brass (3)</td></tr>
<tr><td>003:062</td><td>SH-5 Brass (3)</td></tr>
<tr><td>004:062</td><td>MKS Brass (3)</td></tr>
<tr><td>008:062</td><td>Synth Brass 3 (1)</td></tr>
<tr><td>009:062</td><td>Quack Brass (2)</td></tr>
<tr><td>016:062</td><td>Analog Brass 1 (1)</td></tr>
<tr><td>017:062</td><td>Hybrid Brass (3)</td></tr>
<tr><td>001:063</td><td>Soft Brass (2)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>002:063</td><td>Warm Brass (3)</td></tr>
<tr><td>008:063</td><td>Synth Brass 4 (1)</td></tr>
<tr><td>009:063</td><td>OB Brass (3)</td></tr>
<tr><td>010:063</td><td>Reso Brass (3)</td></tr>
<tr><td>016:063</td><td>Analog Brass 2 (1)</td></tr>
<tr><td>017:063</td><td>Velo Brass 2 (2)</td></tr>
<tr><td>008:064</td><td>Soprano Exp. (3)</td></tr>
<tr><td>008:065</td><td>Hyper Alto (2)</td></tr>
<tr><td>009:065</td><td>Grow Sax (3)</td></tr>
<tr><td>016:065</td><td>Alto Sax + Trumpet (3)</td></tr>
<tr><td>001:066</td><td>Tenor Sax (3 l)</td></tr>
<tr><td>008:066</td><td>Breathy Tenor (2)</td></tr>
<tr><td>009:066</td><td>St. Tenor Sax (3)</td></tr>
<tr><td>001:067</td><td>Baritone Sax (3 l)</td></tr>
<tr><td>008:068</td><td>Oboe Exp. (3)</td></tr>
<tr><td>016:068</td><td>Multi Reed (3)</td></tr>
<tr><td>008:071</td><td>Bass Clarinet (2)</td></tr>
<tr><td>016:071</td><td>Multi Wind (3)</td></tr>
<tr><td>001:072</td><td>Piccolo (3 l)</td></tr>
<tr><td>008:072</td><td>Nay (3)</td></tr>
<tr><td>009:072</td><td>Nay Tremolo (3)</td></tr>
<tr><td>016:072</td><td>Di (3)</td></tr>
<tr><td>001:073</td><td>Flute 2 (3 l)</td></tr>
<tr><td>002:073</td><td>Flute Exp. (3)</td></tr>
<tr><td>003:073</td><td>Flute Travelso (3)</td></tr>
<tr><td>008:073</td><td>Flute + Violin (3)</td></tr>
<tr><td>016:073</td><td>Tron Flute (3)</td></tr>
<tr><td>008:075</td><td>Kawala (2)</td></tr>
<tr><td>016:075</td><td>Zampona (3)</td></tr>
<tr><td>017:075</td><td>Zampona Atk (3)</td></tr>
<tr><td>001:077</td><td>Shakuhachi (3 l)</td></tr>
<tr><td>001:078</td><td>Whistle 2 (3)</td></tr>
<tr><td>001:080</td><td>Square (1)</td></tr>
<tr><td>002:080</td><td>Hollow Mini (2)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>003:080</td><td>Mellow FM (2)</td></tr>
<tr><td>004:080</td><td>CC Solo (2)</td></tr>
<tr><td>005:080</td><td>Shmoog (2)</td></tr>
<tr><td>006:080</td><td>LM Square (2)</td></tr>
<tr><td>008:080</td><td>Sine Wave (1)</td></tr>
<tr><td>009:080</td><td>Sine Lead (3)</td></tr>
<tr><td>010:080</td><td>KG Lead (3)</td></tr>
<tr><td>016:080</td><td>P5 Square (3)</td></tr>
<tr><td>017:080</td><td>OB Square (3)</td></tr>
<tr><td>018:080</td><td>JP-8 Square (3)</td></tr>
<tr><td>024:080</td><td>Pulse Lead (3)</td></tr>
<tr><td>025:080</td><td>JP8 Pulse Lead 1 (3)</td></tr>
<tr><td>026:080</td><td>JP8 Pulse Lead 2 (3)</td></tr>
<tr><td>027:080</td><td>MG Resonant Pulse (3)</td></tr>
<tr><td>001:081</td><td>Saw (1)</td></tr>
<tr><td>002:081</td><td>Pulse Saw (2)</td></tr>
<tr><td>003:081</td><td>Feline GR (2)</td></tr>
<tr><td>004:081</td><td>Big Lead (2)</td></tr>
<tr><td>005:081</td><td>Velo Lead (2)</td></tr>
<tr><td>006:081</td><td>GR-300 (2)</td></tr>
<tr><td>007:081</td><td>LA Saw (2)</td></tr>
<tr><td>008:081</td><td>Doctor Solo (1)</td></tr>
<tr><td>009:081</td><td>Fat Saw Lead (3)</td></tr>
<tr><td>011:081</td><td>D-50 Fat Saw (3)</td></tr>
<tr><td>016:081</td><td>Waspy Synth (2)</td></tr>
<tr><td>017:081</td><td>PM Lead (3)</td></tr>
<tr><td>018:081</td><td>CS Saw Lead (3)</td></tr>
<tr><td>024:081</td><td>MG Saw 1 (3)</td></tr>
<tr><td>025:081</td><td>MG Saw 2 (3)</td></tr>
<tr><td>026:081</td><td>OB Saw 1 (3)</td></tr>
<tr><td>027:081</td><td>OB Saw 2 (3)</td></tr>
<tr><td>028:081</td><td>D-50 Saw (3)</td></tr>
<tr><td>029:081</td><td>SH-101 Saw (3)</td></tr>
<tr><td>030:081</td><td>CS Saw (3)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>031:081</td><td>MG Saw Lead (3)</td></tr>
<tr><td>032:081</td><td>OB Saw Lead (3)</td></tr>
<tr><td>033:081</td><td>P5 Saw Lead (3)</td></tr>
<tr><td>034:081</td><td>MG unison (3)</td></tr>
<tr><td>035:081</td><td>Octave Saw Lead (3)</td></tr>
<tr><td>040:081</td><td>Sequence Saw 1 (3)</td></tr>
<tr><td>041:081</td><td>Sequence Saw 2 (3)</td></tr>
<tr><td>042:081</td><td>Reso Saw (3)</td></tr>
<tr><td>043:081</td><td>Cheese Saw 1 (3)</td></tr>
<tr><td>044:081</td><td>Cheese Saw 2 (3)</td></tr>
<tr><td>045:081</td><td>Rhythmic Saw (3)</td></tr>
<tr><td>001:082</td><td>Vent Synth (2)</td></tr>
<tr><td>002:082</td><td>Pure Pan Lead (2)</td></tr>
<tr><td>001:083</td><td>TB Lead (3)</td></tr>
<tr><td>008:083</td><td>Mad Lead (3)</td></tr>
<tr><td>008:084</td><td>Distorted Lead (2)</td></tr>
<tr><td>009:084</td><td>Acid Guitar1 (3)</td></tr>
<tr><td>010:084</td><td>Acid Guitar2 (3)</td></tr>
<tr><td>016:084</td><td>P5 Sync Lead (3)</td></tr>
<tr><td>017:084</td><td>Fat Sync Lead (3)</td></tr>
<tr><td>018:084</td><td>Rock Lead (3)</td></tr>
<tr><td>019:084</td><td>5th Deca Sync (3)</td></tr>
<tr><td>020:084</td><td>Dirty Sync (3)</td></tr>
<tr><td>024:084</td><td>JUNO Sub Osc (3)</td></tr>
<tr><td>008:085</td><td>Vox Lead (3)</td></tr>
<tr><td>009:085</td><td>LFO Vox (3)</td></tr>
<tr><td>001:086</td><td>Big Fives (2)</td></tr>
<tr><td>002:086</td><td>5th Lead (3)</td></tr>
<tr><td>003:086</td><td>5th Analog Clav (3)</td></tr>
<tr><td>008:086</td><td>4th Lead (3)</td></tr>
<tr><td>001:087</td><td>Big &amp; Raw (2)</td></tr>
<tr><td>002:087</td><td>Fat &amp; Perky (2)</td></tr>
<tr><td>003:087</td><td>JUNO Rave (3)</td></tr>
<tr><td>004:087</td><td>JP8 Bass Lead 1 (3)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>005:087</td><td>JP8 Bass Lead 2 (3)</td></tr>
<tr><td>006:087</td><td>SH-5 Bass Lead (3)</td></tr>
<tr><td>001:088</td><td>Fantasia 2 (2)</td></tr>
<tr><td>002:088</td><td>New Age Pad (3)</td></tr>
<tr><td>003:088</td><td>Bell Heaven (3)</td></tr>
<tr><td>001:089</td><td>Thick Pad (2)</td></tr>
<tr><td>002:089</td><td>Horn Pad (2)</td></tr>
<tr><td>003:089</td><td>Rotary String (2)</td></tr>
<tr><td>004:089</td><td>Soft Pad (2)</td></tr>
<tr><td>008:089</td><td>Octave Pad (3)</td></tr>
<tr><td>009:089</td><td>Stack Pad (3)</td></tr>
<tr><td>001:090</td><td>80's Polysynth (2)</td></tr>
<tr><td>002:090</td><td>Polysynth 2 (3)</td></tr>
<tr><td>003:090</td><td>Poly King (3)</td></tr>
<tr><td>008:090</td><td>Power Stack (3)</td></tr>
<tr><td>009:090</td><td>Octave Stack (3)</td></tr>
<tr><td>010:090</td><td>Resonant Stack (3)</td></tr>
<tr><td>011:090</td><td>Techno Stack (3)</td></tr>
<tr><td>001:091</td><td>Heaven II (2)</td></tr>
<tr><td>002:091</td><td>SC Heaven (3)</td></tr>
<tr><td>008:091</td><td>Cosmic Voice (3)</td></tr>
<tr><td>009:091</td><td>Auh Vox (3)</td></tr>
<tr><td>010:091</td><td>Auh Auh (3)</td></tr>
<tr><td>011:091</td><td>Vocoderman (3)</td></tr>
<tr><td>001:092</td><td>Soft Bell Pad (3)</td></tr>
<tr><td>002:092</td><td>JP8 Square Pad (3)</td></tr>
<tr><td>003:092</td><td>7th Bell Pad (3)</td></tr>
<tr><td>001:093</td><td>Tine Pad (2)</td></tr>
<tr><td>002:093</td><td>Panner Pad (2)</td></tr>
<tr><td>001:094</td><td>Vox Pad (3)</td></tr>
<tr><td>002:094</td><td>Vox Sweep (3)</td></tr>
<tr><td>008:094</td><td>Horror Pad (3)</td></tr>
<tr><td>001:095</td><td>Polar Pad (2)</td></tr>
<tr><td>008:095</td><td>Converge (2)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>009:095</td><td>Shwimmer (2)</td></tr>
<tr><td>010:095</td><td>Celestial Pd (2)</td></tr>
<tr><td>011:095</td><td>Bag Sweep (3)</td></tr>
<tr><td>001:096</td><td>Harmo Rain (2)</td></tr>
<tr><td>002:096</td><td>African Wood (2)</td></tr>
<tr><td>003:096</td><td>Anklung Pad (3)</td></tr>
<tr><td>004:096</td><td>Rattle Pad (3)</td></tr>
<tr><td>008:096</td><td>Clavi Pad (2)</td></tr>
<tr><td>001:097</td><td>Ancestral (2)</td></tr>
<tr><td>002:097</td><td>Prologue (2)</td></tr>
<tr><td>003:097</td><td>Prologue 2 (3)</td></tr>
<tr><td>004:097</td><td>Hols Strings (3)</td></tr>
<tr><td>008:097</td><td>Rave (2)</td></tr>
<tr><td>001:098</td><td>Synth Mallet (1)</td></tr>
<tr><td>002:098</td><td>Soft Crystal (2)</td></tr>
<tr><td>003:098</td><td>Round Glock (2)</td></tr>
<tr><td>004:098</td><td>Loud Glock (2)</td></tr>
<tr><td>005:098</td><td>Glocken Chime (2)</td></tr>
<tr><td>006:098</td><td>Clear Bells (2)</td></tr>
<tr><td>007:098</td><td>Christmas Bell (2)</td></tr>
<tr><td>008:098</td><td>Vibra Bells (2)</td></tr>
<tr><td>009:098</td><td>Digi Bells (2)</td></tr>
<tr><td>010:098</td><td>Music Bell (3)</td></tr>
<tr><td>011:098</td><td>Analog Bell (3)</td></tr>
<tr><td>016:098</td><td>Choral Bells (2)</td></tr>
<tr><td>017:098</td><td>Air Bells (2)</td></tr>
<tr><td>018:098</td><td>Bell Harp (2)</td></tr>
<tr><td>019:098</td><td>Gamelimba (2)</td></tr>
<tr><td>020:098</td><td>JUNO Bell (3)</td></tr>
<tr><td>001:099</td><td>Warm Atmosphere (2)</td></tr>
<tr><td>002:099</td><td>Nylon Harp (2)</td></tr>
<tr><td>003:099</td><td>Harpvox (2)</td></tr>
<tr><td>004:099</td><td>Hollow Release (2)</td></tr>
<tr><td>005:099</td><td>Nylon + Rhodes (2)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>006:099</td><td>Ambient Pad (2)</td></tr>
<tr><td>007:099</td><td>Invisible (3)</td></tr>
<tr><td>008:099</td><td>Pulsey Key (3)</td></tr>
<tr><td>009:099</td><td>Noise Piano (3)</td></tr>
<tr><td>001:100</td><td>Shining Star (3)</td></tr>
<tr><td>002:100</td><td>OB Stab (3)</td></tr>
<tr><td>008:100</td><td>Organ Bell (3)</td></tr>
<tr><td>001:101</td><td>Goblinson (2)</td></tr>
<tr><td>002:101</td><td>50's Sci-Fi (2)</td></tr>
<tr><td>003:101</td><td>Abduction (3)</td></tr>
<tr><td>004:101</td><td>Auhbient (3)</td></tr>
<tr><td>005:101</td><td>LFO Pad (3)</td></tr>
<tr><td>006:101</td><td>Random String (3)</td></tr>
<tr><td>007:101</td><td>Random Pad (3)</td></tr>
<tr><td>008:101</td><td>Low Birds Pad (3)</td></tr>
<tr><td>009:101</td><td>Falling Down (3)</td></tr>
<tr><td>010:101</td><td>LFO RAVE (3)</td></tr>
<tr><td>011:101</td><td>LFO Horror (3)</td></tr>
<tr><td>012:101</td><td>LFO Techno (3)</td></tr>
<tr><td>013:101</td><td>Alternative (3)</td></tr>
<tr><td>014:101</td><td>UFO FX (3)</td></tr>
<tr><td>015:101</td><td>Gargle Man (3)</td></tr>
<tr><td>016:101</td><td>Sweep FX (3)</td></tr>
<tr><td>001:102</td><td>Echo Bell (1)</td></tr>
<tr><td>002:102</td><td>Echo Pan (1)</td></tr>
<tr><td>003:102</td><td>Echo Pan 2 (2)</td></tr>
<tr><td>004:102</td><td>Big Panner (2)</td></tr>
<tr><td>005:102</td><td>Reso Panner (2)</td></tr>
<tr><td>006:102</td><td>Water Piano (2)</td></tr>
<tr><td>008:102</td><td>Pan Sequence (3)</td></tr>
<tr><td>009:102</td><td>Aqua (3)</td></tr>
<tr><td>001:103</td><td>Star Theme 2 (2)</td></tr>
<tr><td>008:103</td><td>Dream Pad (3)</td></tr>
<tr><td>009:103</td><td>Silky Pad (3)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>016:103</td><td>New Century (3)</td></tr>
<tr><td>017:103</td><td>7th Atmosphere (3)</td></tr>
<tr><td>018:103</td><td>Galaxy Way (3)</td></tr>
<tr><td>001:104</td><td>Sitar 2 (1)</td></tr>
<tr><td>002:104</td><td>Detune Sitar (3)</td></tr>
<tr><td>003:104</td><td>Sitar 3 (3)</td></tr>
<tr><td>008:104</td><td>Tambra (2)</td></tr>
<tr><td>016:104</td><td>Tamboura (2)</td></tr>
<tr><td>001:105</td><td>Muted Banjo (2)</td></tr>
<tr><td>008:105</td><td>Rabab (2)</td></tr>
<tr><td>009:105</td><td>San Xian (3)</td></tr>
<tr><td>016:105</td><td>Gopichant (2)</td></tr>
<tr><td>024:105</td><td>Oud (2)</td></tr>
<tr><td>028:105</td><td>Oud + Strings (3)</td></tr>
<tr><td>032:105</td><td>Pi Pa (3)</td></tr>
<tr><td>001:106</td><td>Tsugaru (2)</td></tr>
<tr><td>008:106</td><td>Synth Shamisen (3)</td></tr>
<tr><td>001:107</td><td>Gu Zheng (3)</td></tr>
<tr><td>008:107</td><td>Taisho Koto (1)</td></tr>
<tr><td>016:107</td><td>Kanoon (2)</td></tr>
<tr><td>019:107</td><td>Kanoon + Choir (3)</td></tr>
<tr><td>024:107</td><td>Octave Harp (3)</td></tr>
<tr><td>008:108</td><td>Sanza (3)</td></tr>
<tr><td>008:109</td><td>Didgeridoo (3)</td></tr>
<tr><td>008:110</td><td>Er Hu (3)</td></tr>
<tr><td>009:110</td><td>Gao Hu (3)</td></tr>
<tr><td>001:111</td><td>Shannai 2 (2)</td></tr>
<tr><td>008:111</td><td>Pungi (2)</td></tr>
<tr><td>016:111</td><td>Hichiriki (2)</td></tr>
<tr><td>024:111</td><td>Mizmar (3)</td></tr>
<tr><td>032:111</td><td>Suona 1 (3)</td></tr>
<tr><td>033:111</td><td>Suona 2 (3)</td></tr>
<tr><td>008:112</td><td>Bonang (2)</td></tr>
<tr><td>009:112</td><td>Gender (2)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>010:112</td><td>Gamelan Gong (2)</td></tr>
<tr><td>011:112</td><td>St. Gamelan (2)</td></tr>
<tr><td>012:112</td><td>Jang Gu (3)</td></tr>
<tr><td>016:112</td><td>RAMA Cymbal (2)</td></tr>
<tr><td>008:113</td><td>Atarigane (2)</td></tr>
<tr><td>016:113</td><td>Tambourine (3)</td></tr>
<tr><td>001:114</td><td>Island Mallet (3)</td></tr>
<tr><td>008:115</td><td>Castanets (1)</td></tr>
<tr><td>016:115</td><td>Angklung (3)</td></tr>
<tr><td>017:115</td><td>Angkl Rhythm (3)</td></tr>
<tr><td>024:115</td><td>Finger Snaps (3)</td></tr>
<tr><td>032:115</td><td>909 Hand Clap (3)</td></tr>
<tr><td>001:116</td><td>Small Taiko (3)</td></tr>
<tr><td>008:116</td><td>Concert BD (1)</td></tr>
<tr><td>016:116</td><td>Jungle BD (3)</td></tr>
<tr><td>017:116</td><td>Techno BD (3)</td></tr>
<tr><td>018:116</td><td>Bounce (3)</td></tr>
<tr><td>001:117</td><td>Real Tom (2)</td></tr>
<tr><td>008:117</td><td>Melodic Tom 2 (1)</td></tr>
<tr><td>009:117</td><td>Rock Tom (2)</td></tr>
<tr><td>016:117</td><td>Rash SD (3)</td></tr>
<tr><td>017:117</td><td>House SD (3)</td></tr>
<tr><td>018:117</td><td>Jungle SD (3)</td></tr>
<tr><td>019:117</td><td>909 SD (3)</td></tr>
<tr><td>008:118</td><td>808 Tom (1)</td></tr>
<tr><td>009:118</td><td>Elec Perc (1)</td></tr>
<tr><td>010:118</td><td>Sine Perc (3)</td></tr>
<tr><td>011:118</td><td>606 Tom (3)</td></tr>
<tr><td>012:118</td><td>909 Tom (3)</td></tr>
<tr><td>001:119</td><td>Reverse Cym2 (2)</td></tr>
<tr><td>002:119</td><td>Reverse Cym3 (3)</td></tr>
<tr><td>008:119</td><td>Reverse Snare 1 (2)</td></tr>
<tr><td>009:119</td><td>Reverse Snare 2 (2)</td></tr>
<tr><td>016:119</td><td>Reverse Kick 1 (2)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>017:119</td><td>Reverse Con BD (2)</td></tr>
<tr><td>024:119</td><td>Reverse Tom 1 (2)</td></tr>
<tr><td>025:119</td><td>Reverse Tom 2 (2)</td></tr>
<tr><td>001:120</td><td>Guitar Cut Noise (1)</td></tr>
<tr><td>002:120</td><td>String Slap (1)</td></tr>
<tr><td>003:120</td><td>Guitar Cut Noise 2 (2)</td></tr>
<tr><td>004:120</td><td>Distorted Cut Noise (2)</td></tr>
<tr><td>005:120</td><td>Bass Slide (2)</td></tr>
<tr><td>006:120</td><td>Pick Scrape (2)</td></tr>
<tr><td>008:120</td><td>Guitar FX Menu (3)</td></tr>
<tr><td>009:120</td><td>Bartok Pizzicato (3)</td></tr>
<tr><td>010:120</td><td>Guitar Slap (3)</td></tr>
<tr><td>011:120</td><td>Chord Stroke (3)</td></tr>
<tr><td>012:120</td><td>Biwa Stroke (3)</td></tr>
<tr><td>013:120</td><td>Biwa Tremolo (3)</td></tr>
<tr><td>001:121</td><td>Flute Key Click (1)</td></tr>
<tr><td>001:122</td><td>Rain (1)</td></tr>
<tr><td>002:122</td><td>Thunder (1)</td></tr>
<tr><td>003:122</td><td>Wind (1)</td></tr>
<tr><td>004:122</td><td>Stream (1)</td></tr>
<tr><td>005:122</td><td>Bubble (2)</td></tr>
<tr><td>006:122</td><td>Wind 2 (3)</td></tr>
<tr><td>016:122</td><td>Pink Noise (3)</td></tr>
<tr><td>017:122</td><td>White Noise (3)</td></tr>
<tr><td>001:123</td><td>Dog (1)</td></tr>
<tr><td>002:123</td><td>Horse-Gallop (1)</td></tr>
<tr><td>003:123</td><td>Bird 2 (1)</td></tr>
<tr><td>004:123</td><td>Kitty (2)</td></tr>
<tr><td>005:123</td><td>Growl (2)</td></tr>
<tr><td>001:124</td><td>Telephone 2 (1)</td></tr>
<tr><td>002:124</td><td>Door Creaking (1)</td></tr>
<tr><td>003:124</td><td>Door Slam (1)</td></tr>
<tr><td>004:124</td><td>Scratch (1)</td></tr>
<tr><td>005:124</td><td>Wind Chimes (2)</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>007:124</td><td>Scratch 2 (2)</td></tr>
<tr><td>008:124</td><td>Scratch Key (3)</td></tr>
<tr><td>009:124</td><td>Tape Rewind (3)</td></tr>
<tr><td>010:124</td><td>Phono Noise (3)</td></tr>
<tr><td>011:124</td><td>MC-500 Beep (3)</td></tr>
<tr><td>001:125</td><td>Car-Engine (1)</td></tr>
<tr><td>002:125</td><td>Car-Stop (1)</td></tr>
<tr><td>003:125</td><td>Car-Pass (1)</td></tr>
<tr><td>004:125</td><td>Car-Crash (1)</td></tr>
<tr><td>005:125</td><td>Siren (2)</td></tr>
<tr><td>006:125</td><td>Train (1)</td></tr>
<tr><td>007:125</td><td>Jetplane (1)</td></tr>
<tr><td>008:125</td><td>Starship (1)</td></tr>
<tr><td>009:125</td><td>Burst Noise (1)</td></tr>
<tr><td>010:125</td><td>Calculating (3)</td></tr>
<tr><td>011:125</td><td>Percussive Bang (3)</td></tr>
<tr><td>001:126</td><td>Laughing (1)</td></tr>
<tr><td>002:126</td><td>Screaming (1)</td></tr>
<tr><td>003:126</td><td>Punch (1)</td></tr>
<tr><td>004:126</td><td>Heart Beat (1)</td></tr>
<tr><td>005:126</td><td>Footsteps (2)</td></tr>
<tr><td>006:126</td><td>Applause 2 (2)</td></tr>
<tr><td>007:126</td><td>Small Club (3)</td></tr>
<tr><td>008:126</td><td>Applause Wave (3)</td></tr>
<tr><td>016:126</td><td>Voice One (3)</td></tr>
<tr><td>017:126</td><td>Voice Two (3)</td></tr>
<tr><td>018:126</td><td>Voice Three (3)</td></tr>
<tr><td>019:126</td><td>Voice Tah (3)</td></tr>
<tr><td>020:126</td><td>Voice Whey (3)</td></tr>
<tr><td>001:127</td><td>Machine Gun (1)</td></tr>
<tr><td>002:127</td><td>Lasergun (1)</td></tr>
<tr><td>003:127</td><td>Explosion (1)</td></tr>
<tr><td>004:127</td><td>Eruption (3)</td></tr>
<tr><td>005:127</td><td>Big Shot (3)</td></tr>
</table>
</div>

Legend:

* 1: SC-55 sound set
* 2: SC-88 sound set
* 3: SC-88 Pro sound set
* w: wet sound
* d: dry sound
* o: opened
* l: legato enabled sound


## The Yamaha XG standard {#doc_xg}


The XG standard is detailed in this [document](files/xg_spec.pdf). The instrument list includes all [General MIDI](#doc_gm) standard instruments and also the following items:

<div class='table-columns'>
<table cellspacing="0" cellpadding="1">
<tr><td>001:000</td><td>Grand PianoK</td></tr>
<tr><td>018:000</td><td>MelloGrP</td></tr>
<tr><td>040:000</td><td>PianoStr</td></tr>
<tr><td>041:000</td><td>Dream</td></tr>
<tr><td>001:001</td><td>Bright Piano K</td></tr>
<tr><td>001:002</td><td>Electric Grand Piano K</td></tr>
<tr><td>032:002</td><td>Det.CP80</td></tr>
<tr><td>040:002</td><td>ElGrPno1</td></tr>
<tr><td>041:002</td><td>ElGrPno2</td></tr>
<tr><td>001:003</td><td>Honky Tonk K</td></tr>
<tr><td>001:004</td><td>Electric Piano 1K</td></tr>
<tr><td>018:004</td><td>MelloEP1</td></tr>
<tr><td>032:004</td><td>Chor.EP1</td></tr>
<tr><td>040:004</td><td>HardEl.P</td></tr>
<tr><td>045:004</td><td>VX El.P1</td></tr>
<tr><td>064:004</td><td>60sEl.P</td></tr>
<tr><td>001:005</td><td>Electric Piano2 K</td></tr>
<tr><td>032:005</td><td>Chor.EP2</td></tr>
<tr><td>033:005</td><td>DX Hard</td></tr>
<tr><td>034:005</td><td>DXLegend</td></tr>
<tr><td>040:005</td><td>DX Phase</td></tr>
<tr><td>041:005</td><td>DX+Analg</td></tr>
<tr><td>042:005</td><td>DXKotoEP</td></tr>
<tr><td>045:005</td><td>VX El.P2</td></tr>
<tr><td>001:006</td><td>Harpsichord K</td></tr>
<tr><td>025:006</td><td>Harpsichord 2</td></tr>
<tr><td>035:006</td><td>Harpsichord 3</td></tr>
<tr><td>001:007</td><td>Clavinet K</td></tr>
<tr><td>027:007</td><td>ClaviWah</td></tr>
<tr><td>064:007</td><td>PulseClv</td></tr>
<tr><td>065:007</td><td>PierceCl</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>064:010</td><td>Orgel</td></tr>
<tr><td>001:011</td><td>VibesK</td></tr>
<tr><td>045:011</td><td>HardVibe</td></tr>
<tr><td>001:012</td><td>Marimba K</td></tr>
<tr><td>064:012</td><td>SineMrmb</td></tr>
<tr><td>097:012</td><td>Balafon2</td></tr>
<tr><td>098:012</td><td>Log Drum</td></tr>
<tr><td>096:014</td><td>Church Bell</td></tr>
<tr><td>097:014</td><td>Carillon</td></tr>
<tr><td>035:015</td><td>Dulcimer 2</td></tr>
<tr><td>096:015</td><td>Cimbalom</td></tr>
<tr><td>097:015</td><td>Santur</td></tr>
<tr><td>032:016</td><td>DetDrwOr</td></tr>
<tr><td>033:016</td><td>60sDrOr1</td></tr>
<tr><td>034:016</td><td>60sDrOr2</td></tr>
<tr><td>035:016</td><td>70sDrOr1</td></tr>
<tr><td>036:016</td><td>DrawOrg2</td></tr>
<tr><td>037:016</td><td>60sDrOr3</td></tr>
<tr><td>038:016</td><td>EvenBar</td></tr>
<tr><td>040:016</td><td>16+2"2/3</td></tr>
<tr><td>064:016</td><td>Organ Ba</td></tr>
<tr><td>065:016</td><td>70sDrOr2</td></tr>
<tr><td>066:016</td><td>CheezOrg</td></tr>
<tr><td>067:016</td><td>DrawOrg3</td></tr>
<tr><td>024:017</td><td>70sPcOr1</td></tr>
<tr><td>032:017</td><td>DetPrcOr</td></tr>
<tr><td>033:017</td><td>LiteOrg</td></tr>
<tr><td>037:017</td><td>PercOrg2</td></tr>
<tr><td>064:018</td><td>RotaryOr</td></tr>
<tr><td>065:018</td><td>SlwRotar</td></tr>
<tr><td>066:018</td><td>FstRotar</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>032:019</td><td>Church Organ 3</td></tr>
<tr><td>035:019</td><td>Church Organ 2</td></tr>
<tr><td>040:019</td><td>NotreDame organ</td></tr>
<tr><td>064:019</td><td>Organ Flute</td></tr>
<tr><td>065:019</td><td>TrmOrgFl</td></tr>
<tr><td>040:020</td><td>Puff Org</td></tr>
<tr><td>032:021</td><td>AccordIt</td></tr>
<tr><td>032:022</td><td>Harmo 2</td></tr>
<tr><td>064:023</td><td>TngoAcd2</td></tr>
<tr><td>016:024</td><td>NylonGt2</td></tr>
<tr><td>025:024</td><td>NylonGt3</td></tr>
<tr><td>043:024</td><td>VelGtHrm</td></tr>
<tr><td>096:024</td><td>Ukelele</td></tr>
<tr><td>016:025</td><td>SteelGt2</td></tr>
<tr><td>035:025</td><td>12StrGtr</td></tr>
<tr><td>040:025</td><td>Nyln&Stl</td></tr>
<tr><td>041:025</td><td>Stl&Body</td></tr>
<tr><td>096:025</td><td>Mandolin</td></tr>
<tr><td>018:026</td><td>MelloGtr</td></tr>
<tr><td>032:026</td><td>JazzAmp</td></tr>
<tr><td>032:027</td><td>ChorusGt</td></tr>
<tr><td>040:028</td><td>FunkGtr1</td></tr>
<tr><td>041:028</td><td>MuteStlG</td></tr>
<tr><td>043:028</td><td>FunkGtr2</td></tr>
<tr><td>045:028</td><td>Jazz Man</td></tr>
<tr><td>043:029</td><td>Gt.Pinch</td></tr>
<tr><td>040:030</td><td>FeedbkGt</td></tr>
<tr><td>041:030</td><td>FeedbGt2</td></tr>
<tr><td>065:031</td><td>GtFeedbk</td></tr>
<tr><td>066:031</td><td>GtrHrmo2</td></tr>
<tr><td>040:032</td><td>Jazz Rhythm</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>045:032</td><td>VX Upright</td></tr>
<tr><td>018:033</td><td>FingrDrk</td></tr>
<tr><td>027:033</td><td>FlangeBa</td></tr>
<tr><td>040:033</td><td>Ba&DstEG</td></tr>
<tr><td>043:033</td><td>FngrSlap</td></tr>
<tr><td>045:033</td><td>FngBass2</td></tr>
<tr><td>065:033</td><td>ModAlem</td></tr>
<tr><td>028:034</td><td>MutePkBa</td></tr>
<tr><td>032:035</td><td>Fretless 2</td></tr>
<tr><td>033:035</td><td>Fretless 3</td></tr>
<tr><td>034:035</td><td>Fretless 4</td></tr>
<tr><td>096:035</td><td>SynFretl</td></tr>
<tr><td>097:035</td><td>Smooth</td></tr>
<tr><td>027:036</td><td>ResoSlap</td></tr>
<tr><td>032:036</td><td>PunchThm</td></tr>
<tr><td>043:037</td><td>VeloSlap</td></tr>
<tr><td>018:038</td><td>SynBa1Dk</td></tr>
<tr><td>020:038</td><td>FastResB</td></tr>
<tr><td>024:038</td><td>AcidBass</td></tr>
<tr><td>035:038</td><td>Clv Bass</td></tr>
<tr><td>040:038</td><td>Techno Bass</td></tr>
<tr><td>064:038</td><td>Oscar</td></tr>
<tr><td>065:038</td><td>SqrBass</td></tr>
<tr><td>066:038</td><td>RubberBa</td></tr>
<tr><td>096:038</td><td>Hammer</td></tr>
<tr><td>006:039</td><td>MelloSB1</td></tr>
<tr><td>012:039</td><td>Seq Bass</td></tr>
<tr><td>018:039</td><td>SynBa1Dk</td></tr>
<tr><td>019:039</td><td>SynBa2Dk</td></tr>
<tr><td>032:039</td><td>SmthBa 2</td></tr>
<tr><td>040:039</td><td>Modular Bass</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>041:039</td><td>DX Bass</td></tr>
<tr><td>064:039</td><td>X WireBa</td></tr>
<tr><td>008:040</td><td>SlowVln</td></tr>
<tr><td>018:040</td><td>ClkSynBa</td></tr>
<tr><td>008:044</td><td>SlowTrStr</td></tr>
<tr><td>040:044</td><td>Susp Str</td></tr>
<tr><td>040:046</td><td>Yang Chin</td></tr>
<tr><td>003:048</td><td>S.Strngs</td></tr>
<tr><td>008:048</td><td>SlowStr</td></tr>
<tr><td>024:048</td><td>ArcoStr</td></tr>
<tr><td>035:048</td><td>60sStrng</td></tr>
<tr><td>040:048</td><td>Orchestra</td></tr>
<tr><td>041:048</td><td>Orchstr2</td></tr>
<tr><td>042:048</td><td>TremOrch</td></tr>
<tr><td>045:048</td><td>VeloStr</td></tr>
<tr><td>003:049</td><td>S.SlwStr</td></tr>
<tr><td>008:049</td><td>LegatoSt</td></tr>
<tr><td>040:049</td><td>Warm Str</td></tr>
<tr><td>041:049</td><td>Kingdom</td></tr>
<tr><td>064:049</td><td>70s Str</td></tr>
<tr><td>027:050</td><td>ResoStr</td></tr>
<tr><td>064:050</td><td>Syn Str4</td></tr>
<tr><td>065:050</td><td>SS Str</td></tr>
<tr><td>003:052</td><td>S.Choir</td></tr>
<tr><td>016:052</td><td>Ch.Aahs2</td></tr>
<tr><td>032:052</td><td>MelChoir</td></tr>
<tr><td>040:052</td><td>ChoirStr</td></tr>
<tr><td>040:054</td><td>SynVox2</td></tr>
<tr><td>041:054</td><td>Choral</td></tr>
<tr><td>064:054</td><td>AnaVoice</td></tr>
<tr><td>035:055</td><td>OrchHit2</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>064:055</td><td>Impact</td></tr>
<tr><td>016:056</td><td>Trumpet2</td></tr>
<tr><td>017:056</td><td>BriteTrp</td></tr>
<tr><td>032:056</td><td>WarmTrp</td></tr>
<tr><td>018:057</td><td>Trmbone2</td></tr>
<tr><td>016:058</td><td>Tuba 2</td></tr>
<tr><td>006:060</td><td>FrHrSolo</td></tr>
<tr><td>032:060</td><td>FrHorn2</td></tr>
<tr><td>037:060</td><td>HornOrch</td></tr>
<tr><td>035:061</td><td>Tp&TbSec</td></tr>
<tr><td>040:061</td><td>BrassSec2</td></tr>
<tr><td>041:061</td><td>HiBrass</td></tr>
<tr><td>042:061</td><td>MelloBrs</td></tr>
<tr><td>012:062</td><td>QuackBr</td></tr>
<tr><td>020:062</td><td>RezSynBr</td></tr>
<tr><td>024:062</td><td>PolyBrss</td></tr>
<tr><td>027:062</td><td>SynBras3</td></tr>
<tr><td>032:062</td><td>JumpBrss</td></tr>
<tr><td>045:062</td><td>AnaVelBr</td></tr>
<tr><td>064:062</td><td>AnaBrss1</td></tr>
<tr><td>018:063</td><td>Soft Brs</td></tr>
<tr><td>040:063</td><td>SynBras4</td></tr>
<tr><td>041:063</td><td>ChorBrss</td></tr>
<tr><td>045:063</td><td>VelBras2</td></tr>
<tr><td>064:063</td><td>AnaBras2</td></tr>
<tr><td>040:065</td><td>Sax Section</td></tr>
<tr><td>043:065</td><td>HyprAlto</td></tr>
<tr><td>040:066</td><td>BrthTnSx</td></tr>
<tr><td>041:066</td><td>SoftTenr</td></tr>
<tr><td>064:066</td><td>TnrSax 2</td></tr>
<tr><td>006:080</td><td>Square 2</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>008:080</td><td>LMSquare</td></tr>
<tr><td>018:080</td><td>Hollow</td></tr>
<tr><td>019:080</td><td>Shmoog</td></tr>
<tr><td>064:080</td><td>Mellow</td></tr>
<tr><td>065:080</td><td>SoloSine</td></tr>
<tr><td>066:080</td><td>SineLead</td></tr>
<tr><td>006:081</td><td>Saw 2</td></tr>
<tr><td>008:081</td><td>ThickSaw</td></tr>
<tr><td>018:081</td><td>DynaSaw</td></tr>
<tr><td>019:081</td><td>DigiSaw</td></tr>
<tr><td>020:081</td><td>Big Lead</td></tr>
<tr><td>024:081</td><td>HeavySyn</td></tr>
<tr><td>025:081</td><td>WaspySyn</td></tr>
<tr><td>040:081</td><td>Pulse Saw</td></tr>
<tr><td>041:081</td><td>Dr. Lead</td></tr>
<tr><td>045:081</td><td>VeloLead</td></tr>
<tr><td>096:081</td><td>Seq Ana</td></tr>
<tr><td>065:082</td><td>Pure Pad</td></tr>
<tr><td>064:083</td><td>Rubby</td></tr>
<tr><td>064:084</td><td>DistLead</td></tr>
<tr><td>065:084</td><td>WireLead</td></tr>
<tr><td>024:085</td><td>SynthAah</td></tr>
<tr><td>064:085</td><td>VoxLead</td></tr>
<tr><td>035:086</td><td>Big Five</td></tr>
<tr><td>016:087</td><td>Big&Low</td></tr>
<tr><td>064:087</td><td>Fat&Prky</td></tr>
<tr><td>065:087</td><td>SoftWurl</td></tr>
<tr><td>064:088</td><td>Fantasy2</td></tr>
<tr><td>016:089</td><td>ThickPad</td></tr>
<tr><td>017:089</td><td>Soft Pad</td></tr>
<tr><td>018:089</td><td>SinePad</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>064:089</td><td>Horn Pad</td></tr>
<tr><td>065:089</td><td>RotarStr</td></tr>
<tr><td>064:090</td><td>PolyPd80</td></tr>
<tr><td>065:090</td><td>ClickPad</td></tr>
<tr><td>066:090</td><td>Ana Pad</td></tr>
<tr><td>067:090</td><td>SquarPad</td></tr>
<tr><td>064:091</td><td>Heaven2</td></tr>
<tr><td>066:091</td><td>Itopia</td></tr>
<tr><td>067:091</td><td>CC Pad</td></tr>
<tr><td>064:092</td><td>Glacier</td></tr>
<tr><td>065:092</td><td>GlassPad</td></tr>
<tr><td>064:093</td><td>Tine Pad</td></tr>
<tr><td>065:093</td><td>Pan Pad</td></tr>
<tr><td>020:095</td><td>Shwimmer</td></tr>
<tr><td>027:095</td><td>Converge</td></tr>
<tr><td>064:095</td><td>PolarPad</td></tr>
<tr><td>066:095</td><td>Celstial</td></tr>
<tr><td>045:096</td><td>ClaviPad</td></tr>
<tr><td>064:096</td><td>HrmoRain</td></tr>
<tr><td>065:096</td><td>AfrcnWod</td></tr>
<tr><td>066:096</td><td>Caribean</td></tr>
<tr><td>027:097</td><td>Prologue</td></tr>
<tr><td>064:097</td><td>Ancestrl</td></tr>
<tr><td>012:098</td><td>SynDrCmp</td></tr>
<tr><td>014:098</td><td>Popcorn</td></tr>
<tr><td>018:098</td><td>TinyBell</td></tr>
<tr><td>035:098</td><td>RndGlock</td></tr>
<tr><td>040:098</td><td>GlockChi</td></tr>
<tr><td>041:098</td><td>ClearBell</td></tr>
<tr><td>042:098</td><td>ChorBell</td></tr>
<tr><td>064:098</td><td>SynMalet</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>065:098</td><td>SftCryst</td></tr>
<tr><td>066:098</td><td>LoudGlok</td></tr>
<tr><td>067:098</td><td>XmasBell</td></tr>
<tr><td>068:098</td><td>VibeBell</td></tr>
<tr><td>069:098</td><td>DigiBell</td></tr>
<tr><td>070:098</td><td>AirBells</td></tr>
<tr><td>071:098</td><td>BellHarp</td></tr>
<tr><td>072:098</td><td>Gamelmba</td></tr>
<tr><td>018:099</td><td>WarmAtms</td></tr>
<tr><td>019:099</td><td>HollwRls</td></tr>
<tr><td>040:099</td><td>NylonEP</td></tr>
<tr><td>064:099</td><td>NylnHarp</td></tr>
<tr><td>065:099</td><td>Harp Vox</td></tr>
<tr><td>066:099</td><td>AtmosPad</td></tr>
<tr><td>067:099</td><td>Planet</td></tr>
<tr><td>064:100</td><td>FantaBell</td></tr>
<tr><td>096:100</td><td>Smokey</td></tr>
<tr><td>064:101</td><td>GobSyn</td></tr>
<tr><td>065:101</td><td>50sSciFi</td></tr>
<tr><td>066:101</td><td>Ring Pad</td></tr>
<tr><td>067:101</td><td>Ritual</td></tr>
<tr><td>068:101</td><td>ToHeaven</td></tr>
<tr><td>070:101</td><td>Night</td></tr>
<tr><td>071:101</td><td>Glisten</td></tr>
<tr><td>096:101</td><td>BellChoir</td></tr>
<tr><td>008:102</td><td>EchoPad2</td></tr>
<tr><td>014:102</td><td>Echo Pan</td></tr>
<tr><td>064:102</td><td>EchoBell</td></tr>
<tr><td>065:102</td><td>Big Pan</td></tr>
<tr><td>066:102</td><td>SynPiano</td></tr>
<tr><td>067:102</td><td>Creation</td></tr>
</table>
<table cellspacing="0" cellpadding="1">
<tr><td>068:102</td><td>Stardust</td></tr>
<tr><td>069:102</td><td>Reso Pan</td></tr>
<tr><td>064:103</td><td>Starz</td></tr>
<tr><td>032:104</td><td>DetSitar</td></tr>
<tr><td>035:104</td><td>Sitar 2</td></tr>
<tr><td>096:104</td><td>Tambra</td></tr>
<tr><td>097:104</td><td>Tamboura</td></tr>
<tr><td>028:105</td><td>MuteBnjo</td></tr>
<tr><td>096:105</td><td>Rabab</td></tr>
<tr><td>097:105</td><td>Gopichnt</td></tr>
<tr><td>098:105</td><td>Oud</td></tr>
<tr><td>096:107</td><td>T.Koto</td></tr>
<tr><td>097:107</td><td>Kanoon</td></tr>
<tr><td>064:111</td><td>Shanai2</td></tr>
<tr><td>096:111</td><td>Pungi</td></tr>
<tr><td>097:111</td><td>Hichriki</td></tr>
<tr><td>096:112</td><td>Bonang</td></tr>
<tr><td>097:112</td><td>Gender</td></tr>
<tr><td>098:112</td><td>Gamelan</td></tr>
<tr><td>099:112</td><td>S.Gamlan</td></tr>
<tr><td>100:112</td><td>Rama Cym</td></tr>
<tr><td>101:112</td><td>AsianBell</td></tr>
<tr><td>097:114</td><td>GlasPerc</td></tr>
<tr><td>098:114</td><td>ThaiBell</td></tr>
<tr><td>096:115</td><td>Castanets</td></tr>
<tr><td>096:116</td><td>Gr.Cassa</td></tr>
<tr><td>064:117</td><td>Mel Tom2</td></tr>
<tr><td>065:117</td><td>Real Tom</td></tr>
<tr><td>066:117</td><td>Rock Tom</td></tr>
<tr><td>064:118</td><td>Ana Tom</td></tr>
<tr><td>065:118</td><td>ElecPerc</td></tr>
</table>
</div>


*[MIDI]: Musical Instrument Digital Interface