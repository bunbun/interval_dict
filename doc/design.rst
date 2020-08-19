Motivation from Finance
========================
Financial data is often provided per company or security every day or at different
intervals: for example, `stock prices per company ticker per day`.

Unfortunately, company/security tagging are generally inconsistent, and error strewn.
*"Identifier mapping"* is one of the major challenges in using financial
data

**interval_dict** provides the conceptual framework and building blocks to solve
the main challenges in "Identifier Mapping".

Challenges in identifer mapping
-------------------------------
The nature of the beast:
________________________
Each data source tends to have its own proprietary identifiers, as well
as a set of time-dependent linkages to common third-party identifiers.
These common identifiers can then be used to link together multiple data
sources. So (1) stock prices can be linked to the (2) credit worthiness,
(3) supply chain and (4) geographic exposure of a company.

Common third-party identifiers
________________________________
Examples of common third-party identifiers include

1) `CUSIP <https://en.wikipedia.org/wiki/CUSIP>`_ (licensed from `S&P <https://en.wikipedia.org/wiki/S%26P_Global_Ratings>`_)
2) `SEDOL <https://en.wikipedia.org/wiki/SEDOL>`_ (licensed from `LSE <https://en.wikipedia.org/wiki/London_Stock_Exchange>`_)
3) `ISIN <https://en.wikipedia.org/wiki/International_Securities_Identification_Number>`_ (licensed from `S&P <https://en.wikipedia.org/wiki/S%26P_Global_Ratings>`_)
4) `RIC <https://en.wikipedia.org/wiki/Reuters_Instrument_Code>`_ (licensed from `Reuters <https://en.wikipedia.org/wiki/Reuters_Instrument_Code>`_)
5) `FSYM` (licensed from `Factset <https://en.wikipedia.org/wiki/FactSet>`_)
6) `FIGI` (formerly Bloomberg Global Identifier (BBGID), now an open standard)

Common problems
________________

1) Gaps
2) Offsets
3) Inadequate history
4) Inconsistencies / mapping errors
5) Poor coverage
