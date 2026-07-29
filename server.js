 const express = require('express');
const app = express();

app.use(express.json());

// Lista de productos en memoria (sin intentar escribir en data.json)
let products = [
  "Hellstar Tee - $120",
  "BAPE Sta Tee - $110",
  "Cap Collection (Barbas/Chucky) - $45",
  "Limited Drop Hoodie - $150"
];

// Obtener productos
app.get('/api/products', (req, res) => {
  res.json({ products });
});

// Desactivar el timeout de SSE para Vercel
app.get('/events', (req, res) => {
  res.status(200).send('OK');
});

// Agregar producto sin escribir en el sistema de archivos
app.post('/api/products', (req, res) => {
  const apiKey = req.headers['x-api-key'];
  if (apiKey !== process.env.ADMIN_API_KEY && apiKey !== 'secreto123') {
    return res.status(401).json({ ok: false, error: 'Clave de API incorrecta' });
  }

  const { product } = req.body;
  if (!product) {
    return res.status(400).json({ ok: false, error: 'Producto vacío' });
  }

  products.push(product);
  res.json({ ok: true });
});

// Eliminar producto
app.delete('/api/products/:index', (req, res) => {
  const apiKey = req.headers['x-api-key'];
  if (apiKey !== process.env.ADMIN_API_KEY && apiKey !== 'secreto123') {
    return res.status(401).json({ ok: false, error: 'Clave de API incorrecta' });
  }

  const idx = parseInt(req.params.index) - 1;
  if (idx >= 0 && idx < products.length) {
    products.splice(idx, 1);
    return res.json({ ok: true });
  }
  res.status(400).json({ ok: false, error: 'Índice no válido' });
});

module.exports = app;

const PORT = process.env.PORT || 3000;
if (require.main === module) {
  app.listen(PORT, () => console.log(`Servidor escuchando en puerto ${PORT}`));
}
