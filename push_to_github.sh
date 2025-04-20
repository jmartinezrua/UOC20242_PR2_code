#!/bin/bash

# Obtener el timestamp actual
timestamp=$(date +%s)

# Mensaje del commit con la etiqueta del timestamp
commit_message="Commit automático con timestamp [$timestamp]"

# Agregar todos los cambios al área de preparación
git add .

# Crear el commit con el mensaje
git commit -m "$commit_message"

# Hacer push al repositorio remoto
git push origin main

# Confirmar el resultado
echo "Commit y push realizados con la etiqueta: $timestamp"