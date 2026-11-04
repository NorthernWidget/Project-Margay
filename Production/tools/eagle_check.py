"""Eagle sch/brd consistency: every part's attributes in the schematic against its board element.

Prints the number of mismatches (U$2, a board-only element, is expected) and U13's attributes.

Usage: python3 Production/tools/eagle_check.py Hardware/Margay.sch Hardware/Margay.brd
"""
import sys, xml.etree.ElementTree as ET
sch, brd = sys.argv[1], sys.argv[2]
S = ET.parse(sch).getroot(); B = ET.parse(brd).getroot()
parts = {p.get('name'): p for p in S.iter('part')}
elems = {e.get('name'): e for e in B.iter('element')}
mism = []
for n, e in elems.items():
    p = parts.get(n)
    if p is None: mism.append((n, 'not in sch')); continue
    pa = {a.get('name'): a.get('value') for a in p.findall('attribute') if a.get('value') is not None}
    ea = {a.get('name'): a.get('value') for a in e.findall('attribute') if a.get('value') is not None}
    for k in set(pa) | set(ea):
        if k in pa and k in ea and pa[k] != ea[k]:
            mism.append((n, k, pa[k], ea[k]))
u = parts['U13']; ue = elems['U13']
print('parts', len(parts), 'elements', len(elems), 'attr mismatches', len(mism))
for m in mism[:20]: print('  ', m)
print('U13 sch', {a.get('name'): a.get('value') for a in u.findall('attribute')})
print('U13 brd', {a.get('name'): a.get('value') for a in ue.findall('attribute') if a.get('value')}, 'value', ue.get('value'))
