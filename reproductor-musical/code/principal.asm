;###############################################################################
;Programa para el pic16f877a que permite reproducir tonadas en un
;speaker
;###############################################################################
  processor pic16f877a
  include <p16f877a.inc>

  org 0
    goto inicio
  org 5

  include "notas_musicales.inc"

;;;;;;;;;;;; Definiciones ;;;;;;;;;;;;


;;;;;;;;;;;; Programa ;;;;;;;;;;;;


inicio:
	call sonar_nota_musical
    goto inicio
  end