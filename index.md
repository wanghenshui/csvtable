# csvtable

just a tool to turn csv into ascii table

for describe field stuff in source code, show off(very needed)


```txt
➜  csvtable git:(dev) ✗ csvtable -f ./test/basic.csv   
+-------------+---------+---------+
| aaaaaaaaaa  | bd      | cccccc  |
+-------------+---------+---------+
| d           | eeeeee  | feeeeee |
+-------------+---------+---------+
| "h h h h h" | i(iiii) | j       |
+-------------+---------+---------+
```


```txt
➜  csvtable git:(dev) ✗ csvtable -s "term,lsn,timestamp"
+------+-----+-----------+
| term | lsn | timestamp |
+------+-----+-----------+
```


## how to build

install xmake then

```bash
xmake
```

binary file in `csvtable/build/linux/x86_64/release/`

## TODO
support stream like `cat xxx | csvtable`

