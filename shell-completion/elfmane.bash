_elfmane() {
  local cur="${COMP_WORDS[COMP_CWORD]}"
  local flags="--help --version"

  # We don't have a file yet.
  # Keep doing filesystem completion.
  if [[ ${COMP_CWORD} -eq 1 || ! -f "${COMP_WORDS[1]}" ]]; then
    compopt -o filenames
    COMPREPLY=($(compgen -f -- "$cur"))
    return
  fi

  # First argument is a real file.
  # Everything after it is a flag.
  COMPREPLY=($(compgen -W "$flags" -- "$cur"))
}

complete -F _elfmane elfmane
