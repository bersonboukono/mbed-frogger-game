## Hi there 👋

<!--
**bersonboukono/bersonboukono** is a ✨ _special_ ✨ repository because its `README.md` (this file) appears on your GitHub profile.

Here are some ideas to get you started:

- 🔭 I’m currently working on ...
- 🌱 I’m currently learning ...
- 👯 I’m looking to collaborate on ...
- 🤔 I’m looking for help with ...
- 💬 Ask me about ...
- 📫 How to reach me: ...
- 😄 Pronouns: ...
- ⚡ Fun fact: ...
-->

<img width="802" height="1101" alt="Diagramme_Frogger_mbed drawio (1)" src="https://github.com/user-attachments/assets/c3699952-a133-4668-80c5-ab599a4a1c2e" />

# Développement et Architecture

Ce projet a été développé en C/C++ et compilé via Keil Studio Cloud (l'IDE en ligne officiel d'ARM Mbed).

L'architecture du code a été pensée pour être à la fois claire et réactive :

Une logique par étapes : Le programme est découpé en séquences simples (Menu, Jeu en cours, Victoire/Défaite) pour que le microcontrôleur sache toujours exactement quoi afficher.

Une boucle de jeu évolutive : Le cœur du programme tourne en continu pour faire avancer les obstacles. Cette boucle gère automatiquement la progression du joueur sur 3 niveaux de difficulté (accélération de la vitesse, ajout de voitures).

Une gestion matérielle instantanée : Le code est conçu pour réagir immédiatement aux actions du joueur (mouvements du joystick, bouton pause) tout en synchronisant les composants de la carte en temps réel (affichage sur l'écran LCD, mise à jour des LEDs pour les vies, et gestion de la musique de fond).
