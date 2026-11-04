"""Rebuild Eagle's bom.ulp CSV export (list by Value, with attributes) from an Eagle .sch, without Eagle.

Checked against Eagle's own export: run on Hardware/Margay.sch at f2cbf69 (2023-03-17),
it reproduces Production/Margay_BoM_3v0_-_2023-03-17.csv as committed in 6a72e5c,
byte for byte.

Usage: python3 Production/tools/eagle_bom.py Hardware/Margay.sch > BoM.csv
"""
import sys, re, xml.etree.ElementTree as ET
root = ET.parse(sys.argv[1]).getroot()
libs = {}
for lib in root.iter('library'):
    libs[(lib.get('name'), lib.get('urn') or '')] = lib
def find_lib(name, urn):
    for (n, u), l in libs.items():
        if n == name and (u == (urn or '') or not urn): return l
dev_info = {}
rows = {}
for p in root.iter('part'):
    lib = find_lib(p.get('library'), p.get('library_urn'))
    ds = next(d for d in lib.iter('deviceset') if d.get('name') == p.get('deviceset'))
    dev = next(d for d in ds.iter('device') if d.get('name') == p.get('device'))
    if dev.get('package') is None: continue          # supply symbols, frames
    tech = p.get('technology') or ''
    dsn = ds.get('name')
    devname = (dsn.replace('*', tech) if '*' in dsn else dsn + tech)
    devname = devname.replace('?', dev.get('name')) if '?' in devname else devname + dev.get('name')
    attrs = {}
    for t in dev.iter('technology'):
        if t.get('name') == tech:
            attrs.update({a.get('name'): a.get('value') or '' for a in t.findall('attribute')})
    attrs.update({a.get('name'): a.get('value') or '' for a in p.findall('attribute')})
    value = p.get('value') if p.get('value') is not None else ('' if ds.get('uservalue') == 'yes' else devname)
    d = ds.find('description'); desc = (d.text or '') if d is not None else ''
    desc = re.sub(r'<p>|<br */?>', '\n', desc, flags=re.I); desc = re.sub(r'<[^>]+>', '', desc).strip().split('\n')[0].strip()
    key = (value, devname, dev.get('package'), desc, tuple(sorted(attrs.items())))
    rows.setdefault(key, []).append(p.get('name'))
names = sorted({k for key in rows for k, _ in key[4]})
nat = lambda s: [int(t) if t.isdigit() else t for t in re.split(r'(\d+)', s)]
out = ['"Qty","Value","Device","Package","Parts","Description",' + ''.join(f'"{n}",' for n in names)]
for key in sorted(rows, key=lambda k: (k[0], k[1])):
    value, devname, pkg, desc, attrs = key; a = dict(attrs)
    parts = ', '.join(sorted(rows[key], key=nat))
    out.append(f'"{len(rows[key])}","{value}","{devname}","{pkg}","{parts}","{desc}",' + ''.join(f'"{a.get(n, "")}",' for n in names))
sys.stdout.buffer.write(('\r\n'.join(out) + '\r\n').encode())
