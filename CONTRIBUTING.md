# Contributing

Thanks for your interest in Monsun.

- Open an issue before large changes. Describe the hardware you tested on.
- Keep portable libs (`firmware/libs`) free of hardware and OS dependencies and covered by host tests.
- Follow `.editorconfig`. C code: C11, 4 spaces, `monsun_` prefix for public symbols.
- Run the host tests before opening a PR.
- Hardware changes: include a wiring description and a safety note (see [docs/safety.md](docs/safety.md)).
- Decisions that change architecture get an ADR in `docs/adr/`.

By contributing you agree your work is licensed under the MIT license.
