# Vendored toolchain

Verbatim copies of the course libraries the compiler builds against, so the
build does not depend on the course wiki staying up. Both are GPLv3 (see the
LICENSE file inside each archive).

| File | What | Original location |
|------|------|-------------------|
| `Libcdk21-202605161833.tar.bz2` | CDK21, the Compiler Development Kit | https://robots.hlt.inesc-id.pt/w/pt/images/5/5f/Libcdk21-202605161833.tar.bz2 |
| `Librts7-202605092235.tar.bz2` | RTS7, the runtime library (`librts`) | https://robots.hlt.inesc-id.pt/w/pt/images/3/39/Librts7-202605092235.tar.bz2 |

The `Dockerfile` checks their SHA-256 before building them:

```
8cdc0e726eea9b89246f94036cf8c2dc4025a331c7170028c9f111633bfd878d  Libcdk21-202605161833.tar.bz2
e89a80ad31630396f14ddf1dccfbff8efd9002985b63cb3692759aaabc670bc0  Librts7-202605092235.tar.bz2
```
