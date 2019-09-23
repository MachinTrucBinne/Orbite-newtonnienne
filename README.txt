2019-09-21

		☾ ORBITE D'UNE PARTICULE PONCTUELLE ☾

######################################################################
######################################################################

		DESCRIPTION

C'est une simulation d'une particule ponctuelle soumise à la force gravitationnelle de deux boules.

La particule peut passer à travers les deux boules.

Hors d'une boule, le potentiel gravitationnel est coulombien :
	V(r) ≈ -1/r

Dans une boule, le potentiel gravitationnel est celui d'un oscillateur harmonique :
	V(r) ≈ r²/2

Codé en novembre 2008.
Adapté pour SDL2 en juin 2017.
Déposé sur GitHub en septembre 2019.

######################################################################
######################################################################

		OS

Ça marche sur macOS Mojave 10.14.6, mais ça devrait se porter aisément à d'autres OS.

######################################################################
######################################################################

		COMPILATION

D'abord, installer Brew.
Ensuite, utiliser Brew pour installer SDL :
	brew install SDL2
	brew install SDL2_image

Le code devrait compiler avec :

gcc $(sdl2-config --cflags) -Wall -o main  main.c $(sdl2-config --libs) -lSDL2_image -framework OpenGL -framework GLUT -Wno-deprecated

######################################################################
######################################################################

		CONTRÔLES

Espace : active/désactive la rotation continue ou non
Entrer : active/désactive l'évolution dynamique de la particule ponctuelle

Gauche/droite : rotation de la vue
Haut/bas : s'approcher/s'éloigner

######################################################################
######################################################################

Noé Aubin-Cadot, 2019.