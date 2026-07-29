Persistencia local (data.json)

El programa C++ ahora guarda y carga productos y pedidos desde data.json en la misma carpeta del ejecutable.

Estructura simple del archivo (JSON):
{
  "products": [
	"Hellstar Tee - $120",
	"BAPE Sta Tee - $110"
  ],
  "orders": [
	"Orden #1001 - Hellstar Tee - Usuario: user1"
  ]
}

Comportamiento:
- Al iniciar, la app carga data.json si existe y reemplaza la lista por defecto.
- Cuando el admin agrega o elimina productos, los cambios se guardan inmediatamente.
- Al salir de la app, se guarda el estado actual en data.json.

Notas de seguridad:
- Este almacenamiento es solo para demostración. No uses este enfoque para datos sensibles en producción.
- Cambia la contraseña admin por defecto (admin123) antes de usar en público.

Cómo ver la tienda web:
- Abre web_store.html en un navegador moderno (puedes servirlo con un servidor estático para evitar restricciones de CORS).
- web_google_signin.html muestra un ejemplo de inicio de sesión con Google (requiere reemplazar el Client ID).
