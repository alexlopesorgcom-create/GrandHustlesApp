const express = require('express');
const fs = require('fs');
const path = require('path');
const cors = require('cors');

const app = express();
app.use(cors());
app.use(express.static(path.join(__dirname)));
app.use(express.json());

// Simple API key for admin operations (override with env var)
const API_KEY = process.env.API_KEY || 'admin123';

const DATA_FILE = path.join(__dirname, 'data.json');
const PORT = process.env.PORT || 5500;

function readProducts() {
  try {
	const s = fs.readFileSync(DATA_FILE, 'utf8');
	const obj = JSON.parse(s);
	const products = Array.isArray(obj.products) ? obj.products : [];
	// Normalize products: if an object has an empty or missing image field,
	// set a server-side default so clients can use /images/placeholder.jpg
	return products.map(p => {
	  if (p && typeof p === 'object') {
		if (!p.image || (typeof p.image === 'string' && p.image.trim() === '')) {
		  return Object.assign({}, p, { image: '/images/placeholder.jpg' });
		}
	  }
	  return p;
	});
  } catch (e) {
	return [];
  }
}

app.get('/api/products', (req, res) => {
  res.json({ products: readProducts() });
});

// Protected endpoint: add a product
app.post('/api/products', (req, res) => {
  const key = req.headers['x-api-key'] || req.query.api_key;
  if (key !== API_KEY) return res.status(401).json({ error: 'Unauthorized' });

  const body = req.body || {};
  let product = null;
  if (typeof body.product === 'string') product = body.product;
  else if (typeof body.title === 'string') product = body.title + (body.price ? ' - ' + body.price : '');
  if (!product) return res.status(400).json({ error: 'Invalid product' });

  const data = fs.existsSync(DATA_FILE) ? JSON.parse(fs.readFileSync(DATA_FILE, 'utf8')) : { products: [], orders: [] };
  data.products = Array.isArray(data.products) ? data.products : [];
  data.products.push(product);
  fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2), 'utf8');
  notifyClients();
  res.json({ ok: true, product });
});

// Protected endpoint: delete product by index (1-based)
app.delete('/api/products/:index', (req, res) => {
  const key = req.headers['x-api-key'] || req.query.api_key;
  if (key !== API_KEY) return res.status(401).json({ error: 'Unauthorized' });

  const idx = parseInt(req.params.index, 10);
  if (isNaN(idx) || idx < 1) return res.status(400).json({ error: 'Invalid index' });

  const data = fs.existsSync(DATA_FILE) ? JSON.parse(fs.readFileSync(DATA_FILE, 'utf8')) : { products: [], orders: [] };
  data.products = Array.isArray(data.products) ? data.products : [];
  if (idx > data.products.length) return res.status(400).json({ error: 'Index out of range' });
  const removed = data.products.splice(idx - 1, 1);
  fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2), 'utf8');
  notifyClients();
  res.json({ ok: true, removed });
});

// Server-Sent Events (SSE) to notify clients when data.json changes
let clients = [];

app.get('/events', (req, res) => {
  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');
  res.flushHeaders && res.flushHeaders();

  // send a comment to keep the connection alive
  res.write(': connected\n\n');

  clients.push(res);

  req.on('close', () => {
	clients = clients.filter(c => c !== res);
  });
});

function notifyClients() {
  clients.forEach(res => {
	try {
	  res.write('event: update\n');
	  res.write('data: update\n\n');
	} catch (e) {
	  // ignore write errors
	}
  });
}

// Watch data.json for changes. Use fs.watchFile for compatibility.
if (fs.existsSync(DATA_FILE)) {
  fs.watchFile(DATA_FILE, { interval: 500 }, (curr, prev) => {
	if (curr.mtimeMs !== prev.mtimeMs) {
	  notifyClients();
	}
  });
} else {
  // ensure file exists so watchers can be attached
  fs.writeFileSync(DATA_FILE, JSON.stringify({ products: [], orders: [] }, null, 2), 'utf8');
  fs.watchFile(DATA_FILE, { interval: 500 }, (curr, prev) => {
	if (curr.mtimeMs !== prev.mtimeMs) notifyClients();
  });
}

app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}/`);
});
