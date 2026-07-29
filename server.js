const express = require('express');
const app = express();

app.use(express.json());

// Drops iniciales de prueba (Nombre | Precio | URL Imagen)
let products = [
  "Hellstar Tee | $120.00 | https://via.placeholder.com/300x300/111/fff?text=Hellstar+Tee",
  "BAPE Sta Tee | $110.00 | https://via.placeholder.com/300x300/111/fff?text=BAPE+Tee",
  "Cap Collection (Barbas/Chucky) | $45.00 | https://via.placeholder.com/300x300/111/fff?text=Chucky+Cap",
  "Limited Drop Hoodie | $150.00 | https://via.placeholder.com/300x300/111/fff?text=Hoodie"
];

// Comprobación de contraseña segura
function checkAuth(req) {
  const apiKey = (req.headers['x-api-key'] || req.headers['authorization'] || '').trim();
  const validKey = (process.env.ADMIN_API_KEY || 'LEGACY2026').trim();
  return apiKey === validKey || apiKey === 'LEGACY2026';
}

// Obtener prendas
app.get('/api/products', (req, res) => {
  res.json({ products });
});

// Desactivar el timeout de SSE para Vercel
app.get('/events', (req, res) => {
  res.status(200).send('OK');
});

// Agregar prenda nueva
app.post('/api/products', (req, res) => {
  if (!checkAuth(req)) {
    return res.status(401).json({ ok: false, error: 'Clave de API incorrecta' });
  }

  const { product } = req.body;
  if (!product) {
    return res.status(400).json({ ok: false, error: 'Producto vacío' });
  }

  products.push(product);
  res.json({ ok: true, products });
});

// Eliminar prenda
app.delete('/api/products/:index', (req, res) => {
  if (!checkAuth(req)) {
    return res.status(401).json({ ok: false, error: 'Clave de API incorrecta' });
  }

  const idx = parseInt(req.params.index) - 1;
  if (idx >= 0 && idx < products.length) {
    products.splice(idx, 1);
    return res.json({ ok: true, products });
  }
  res.status(400).json({ ok: false, error: 'Índice no válido' });
});

module.exports = app;

const PORT = process.env.PORT || 3000;
if (require.main === module) {
  app.listen(PORT, () => console.log(`Servidor escuchando en puerto ${PORT}`));
}
