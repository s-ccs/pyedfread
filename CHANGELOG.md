# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.3.0] - 2024-07-19
### Major
So far, blink-events were not explicitly returned. Rather, a `blink` column existed, indicating that within an event, a blink occurred. Two breaking changes:
    - `blink` renamed to `contains_blink`
    - Blink events are returned explicitly, using `ENDBLINK` event type from Eyelink. I (Benedikt Ehinger) noticed that the ENDBLINK and STARTBLINK start-times differed by 1 sample, but I have no explanation for this and just use the `ENDBLINK` data.
