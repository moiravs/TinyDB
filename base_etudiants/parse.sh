USAGE='script.sh [-d|--dir directory] [-o|--opt options] [filename]'

function parse_args () {
    # for param équivaut à for param in "$@"
    for param ; do

        # command || command exécute la deuxième command seulement si la première retourne non-zéro.
        # La valeur de retour est la valeur de retour de la dernière commande exécutée
        if [ $param = '-h' ] || [ $param = '--help' ] || [ $param = 'help' ] ; then

            # 1>&2 est la façon usuelle de rediriger le stdout (1) vers le stderr (2)
            echo "$USAGE" 1>&2

            # Finit le script
            exit
        fi
    done

    # On peut utiliser shift pour retirer le premier paramètre et décaler tous les autres
    # paramètres d'une place vers le début.
    # On peut donc parcourir tous les paramètres avec while [ $# -gt 0 ]; et shift.
    while [ $# -gt 0 ]; do

        # https://www.gnu.org/software/bash/manual/bash.html#index-case
        # Avant chaque ')' on a une liste de pattern séparé par un '|'.
        # Dans un pattern, une '*' match n'importe quelle suite de caractère (vide ou non) et le '?' match un seul caractère.
        case "$1" in
            -d|--dir)
                if [ $# -lt 2 ] ; then
                    echo '-d|--dir requires an argument' 1>&2
                    exit $BAD_USAGE
                fi
                DIR="$2"
                shift ; shift
                ;;
            -c|--opt)
                if [ $# -lt 2 ] ; then
                    echo '-o|--opt requires an argument' 1>&2
                    exit $BAD_USAGE
                fi
                OPTIONS="$2"
                shift ; shift
                ;;
            -*)
                echo "Option inconnue: $1"
                exit $BAD_USAGE
                ;;
            *)
                FILE_NAME="$1"
                shift
                ;;
        esac
    done
}

parse_args "$@" # Passe les arguments du script à la fonction.
                # Nécessaire car les variables spéciales $#, $@, $*, $1, $2...
                # (que bash appelle paramètres) se réfèrent aux
                # paramètres de la fonction dès qu'ils sont
                # utilisés dans une fonction.
                # NOTE: Ces paramètres ne peuvent pas être modifiés
                # comme des variables usuelles.
                # Pour que les paramètres contenant des espaces
                # ne soient pas coupés, il faut utiliser les ".
                # Bash définit "$@" comme équivalent à "$1" "$2"...
                # et $@ comme équivalent à $1, $2...
