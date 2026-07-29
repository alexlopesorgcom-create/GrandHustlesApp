Servidor Node/Express (instrucciones)

1) Instalar dependencias
   - Abre PowerShell en la carpeta del proyecto (D:\GrandHustlesApp\GrandHustlesApp\)
   - Ejecuta: npm install

2) Iniciar el servidor
   - Ejecuta: npm start
   - El servidor escuchará por defecto en http://localhost:5500/

3) Qué hace
   - Sirve archivos estáticos (web_store.html, web_google_signin.html, etc.)
   - Proporciona API GET /api/products que devuelve { products: [...] } leyendo data.json
   - Expone /events (Server-Sent Events). Cuando data.json cambia, el servidor envía un evento "update" a los clientes conectados.

4) Pruebas
   - Abre http://localhost:5500/web_store.html en tu navegador. La página cargará productos desde /api/products.
   - Ejecuta la app C++ y, desde el panel admin, agrega/elimina productos. Cuando data.json cambie, la web_store.html recibirá el evento y se actualizará automáticamente.

Notas:
- Asegúrate de que data.json se encuentre en la misma carpeta que el servidor (la app C++ guarda data.json allí).
- Para producción, añade validación, control de acceso y CORS según sea necesario.
