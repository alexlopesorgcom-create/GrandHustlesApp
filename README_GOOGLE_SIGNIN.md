Instrucciones rápidas (español)

1) Crear credenciales en Google Cloud Console
   - Ve a https://console.cloud.google.com/apis/credentials
   - Crea un "OAuth 2.0 Client ID" (Tipo: Aplicación web)
   - En "Authorized JavaScript origins" agrega tu origen (por ejemplo: http://localhost:5500)
   - Copia el Client ID

2) Actualizar el ejemplo
   - Abre web_google_signin.html
   - Reemplaza REPLACE_WITH_YOUR_CLIENT_ID.apps.googleusercontent.com por tu Client ID

3) Servir el archivo
   - Para pruebas locales puedes usar un servidor estático (VS Code Live Server, Python -m http.server, etc.)
   - Para producción, sirve desde HTTPS

4) Validación en servidor (opcional pero recomendado)
   - Envía el ID token al backend y valida con la API de Google (https://oauth2.googleapis.com/tokeninfo?id_token=...)

Notas:
- Este ejemplo decodifica el ID token en el cliente solo para demostración; no lo utilices como medida de seguridad en producción.
- Configura correctamente los orígenes autorizados y las URIs de redirección en Google Cloud Console.