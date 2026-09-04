# elfmane
An ELF interpreting CLI tool written in C without any external dependencies.

elfmane reads raw bytes from an ELF file to parse data like ELF Header, Program Segment Headers and Section Headers. elfmane's focus on readability and polishedness makes the tool distinct from a well known alternative readelf. While readelf is more of a general purpose tool with all needed information packed in it, elfmane specifies in having only the essentials for the simple usecases for smoother experience.

This is a project built with the goal of gaining experience in low-level programming in C and being able to maintain a somewhat . Despite this, elfmane is not limited to being a simple learning project and in reality has a problem that it's solving and is intended for use. Therefore elfmane is not just "testing until it works okay" but a piece of software I would use myself and be content.

The project will be maintained until it starts reaching its ceiling and is fully polished.