extern Entries   ; rdi: disk, rsi: entries, rdx: entrysize
extern Write     ; rdi: disk, xmm0: entry, rsi: data, rdx: dataLength
extern Read      ; rdi: disk, xmm0: entry, rdi: data
