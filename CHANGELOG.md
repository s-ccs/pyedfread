# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
## [0.3.2] - 2025-04-15
### Major
Richard Vaelle (@flyingfalling) fixed a major, where prior to this release the returned samples dataframe had too many rows (by the number of events), the values in these rows were undefined/unitialized values which, depending on the system, could be 0's, values close to 0, previous memory-usages o, or any other value. This can be very problematic, if by chance the same memory layout is reused, because then the initialized values could be eyetracking data from a previous call to pyedfread. This should be a quite visible error through.

### Minor
- Some bugfixes to get tests working on windows
- Some small fixes in the readme
- Bugfix for `parse_msg` where a `startwith` was fixed to `startswith`

## [0.3.0] - 2024-07-19
### Major
So far, blink-events were not explicitly returned. Rather, a `blink` column existed, indicating that within an event, a blink occurred. Two breaking changes:
    - `blink` renamed to `contains_blink`
    - Blink events are returned explicitly, using `ENDBLINK` event type from Eyelink. I (Benedikt Ehinger) noticed that the ENDBLINK and STARTBLINK start-times differed by 1 sample, but I have no explanation for this and just use the `ENDBLINK` data.
