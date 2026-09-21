# GET NEXT LINE (GNL) - SAVUNMA VE ÇALIŞMA REHBERİ

Bu dosya, projeyi teslim etmen için değil; projeyi mükemmel bir şekilde anlaman, değerlendirme (defense) sırasında sorulabilecek sorulara hazırlıklı olman ve C dilinin önemli konseptlerini kavramanız için özel olarak hazırlanmıştır.

---

## BÖLÜM 1: MAKEFILE NEDİR VE NASIL ÇALIŞIR?

Her ne kadar GNL tesliminde `Makefile` gerekmese de, 42 okulunun hemen hemen tüm diğer projelerinde kullanacağın bu yapıyı anlamak çok önemlidir.

`Makefile`, projeni derleme (compile) adımlarını otomatize eden bir reçetedir. Dosyaları tek tek `cc -Wall -Wextra ...` diye terminale yazmak yerine `make` yazarsın ve o senin yerine her şeyi derler.

### Temel Kurallar (Rules)
- **`$(NAME)` veya `all`**: Projenin ana hedefini (örneğin `libgnl.a` kütüphanesini veya bir çalıştırılabilir dosyayı) oluşturur. Sadece değişen `.c` dosyalarını yeniden derler, böylece zaman kazandırır (buna "relinking'i engellemek" denir).
- **`clean`**: Derleme sırasında oluşan `.o` (obje) dosyalarını siler. Kaynak kodlarına dokunmaz, sadece ara derleme dosyalarını temizler.
- **`fclean`**: `clean` kuralını çalıştırır, buna ek olarak oluşturulan asıl kütüphaneyi (`libgnl.a`) veya programı siler. Klasörü ilk indirdiğin tertemiz haline getirir.
- **`re`**: Önce `fclean` yapar, sonra tekrar `all` yaparak projeyi sıfırdan tamamen baştan derler.
- **`ar rcs`**: Derlenen `.o` dosyalarını alıp tek bir `.a` (statik kütüphane - arşivi) dosyasında birleştiren komuttur.

---

## BÖLÜM 2: PROJENİN ÇOK DETAYLI ÇALIŞMA MANTIĞI

### Temel Kavramlar
1. **File Descriptor (FD):** İşletim sisteminin açılan bir dosyayı veya veri akışını takip etmek için atadığı negatif olmayan bir tam sayıdır. Standart girdi (klavye) için `0`, ekran çıktısı için `1`, hatalar için `2`'dir. Açtığın dosyalar genellikle `3, 4, 5...` diye devam eder.
2. **BUFFER_SIZE:** `read` fonksiyonunun tek seferde dosyadan kaç byte okuyacağını belirten makrodur. Derleme sırasında `-D BUFFER_SIZE=42` şeklinde dışarıdan verilir.

### Adım Adım GNL Algoritması
Projemiz, dosyadan veriyi tek seferde almak yerine `BUFFER_SIZE` kadar parçalar halinde alır. Fakat satır sonu (`\n`) karakteri her zaman tam o parçanın sonuna denk gelmez.

1. **Zula (Stash) Mantığı - `static char *stash`:** 
   Dosyadan 42 karakter okuduk ancak ilk 10 karakterde satır sonu (`\n`) var. O zaman kalan 32 karakter ne olacak? Çöpe atamayız çünkü onlar bir sonraki satırın başı. İşte bu kalan karakterleri fonksiyon bittikten sonra da bellekte tutabilmek için `static` (statik) bir değişken kullanırız.

2. **Okuma İşlemi (`read_and_stash`):**
   Bir döngü içerisinde `read` fonksiyonuyla dosyadan `BUFFER_SIZE` kadar okuma yaparız. Okuduğumuz veriyi `ft_strjoin` yardımıyla daha önceden elimizde kalan zulanın (`stash`) sonuna ekleriz.
   Döngü ne zaman durur?
   - Zulanın içinde bir `\n` gördüğümüz an.
   - Dosya tamamen bittiğinde (read 0 döndürdüğünde).

3. **Satırı Çekip Alma (`extract_line`):**
   Artık zulamızın (`stash`) içinde kesin olarak bir `\n` (veya dosya sonu) olduğunu biliyoruz. Zulanın en başından başlayıp `\n` karakterine kadar olan kısmı yeni bir `malloc` ile hafızada oluşturup kopyalarız. Kullanıcıya dönecek olan satır budur.

4. **Zulayı Güncelleme ve Bellek Yönetimi (`update_stash`):**
   Kullanıcıya satırı verdik, ancak bu satır hala `stash`'in içinde duruyor. Zulanın içinden o verdiğimiz satırı kesip atmamız lazım.
   İlk `\n`'den sonraki kısmı buluruz, geri kalan bu veriyi yeni bir string içine aktarırız ve **eski büyük stash'i `free` ederiz**. Eğer dosyada başka okunacak hiçbir şey kalmamışsa, stash'i tamamen temizleyip `NULL` yaparız. Bu sayede **Memory Leak (Bellek sızıntısı)** yaşanmaz.

---

## BÖLÜM 3: DEĞERLENDİRME (DEFENSE) SORULARI VE CEVAPLARI

**Soru 1: Statik değişken (Static Variable) nedir? Normal değişkenden farkı nedir? Nerede tutulur?**
*Cevap:* Normal yerel değişkenler fonksiyon çağrıldığında `Stack` bellekte oluşturulur ve fonksiyon bittiğinde silinir. `static` değişkenler ise program çalışmaya başladığında belleğin `Data` veya `BSS` segmentinde oluşturulur ve program kapanana kadar değerini korur. Fonksiyondan çıksak bile içindeki veriyi kaybetmez. GNL'de artan veriyi bir sonraki çağrıya taşımak için kullandık.

**Soru 2: `read` fonksiyonu nasıl çalışır? Geri dönüş değerleri ne anlama gelir?**
*Cevap:* `read(int fd, void *buf, size_t count)` aldığı `fd`'den, `count` kadar byte okuyup `buf` içine yazar.
- `-1` dönerse: Okuma sırasında bir hata oldu demektir. (Örn: Geçersiz bir fd).
- `0` dönerse: Dosyanın sonuna gelinmiş (End Of File - EOF), okunacak bir şey kalmamış demektir.
- `>0` dönerse: Dosyadan başarıyla kaç byte okuduğunu gösterir.

**Soru 3: Memory Leak (Bellek Sızıntısı) nedir? Projende leak oluşmaması için ne yaptın?**
*Cevap:* `malloc` ile RAM'den tahsis edilen belleğin işi bittiğinde `free` edilmeyip program kapanana kadar askıda kalmasıdır. Benim kodumda:
1. `ft_strjoin` içinde iki stringi birleştirdikten sonra eski `stash` stringini her zaman `free` ettim.
2. `update_stash` içinde kalan veriyi alıp kaydettikten sonra, önceki büyük veriyi `free` ettim. 
3. Dosya sonuna gelindiğinde (`\n`'den sonra bir şey kalmadığında) `stash`'i komple `free` edip `NULL` bıraktım.

**Soru 4: Projeyi `-D BUFFER_SIZE=1` ile derlersem ne olur? Ya da `999999` ile derlersem?**
*Cevap:* `BUFFER_SIZE=1` olursa program tek tek karakter okuyacağı için aşırı fazla kez `read` fonksiyonu çağrılır. Bu işletim sistemini yorar ve çok yavaş çalışır ama fonksiyon yine de doğru satırı bulur.
Eğer `999999` yaparsak çok hızlı çalışır ancak stack bellekte veya heap'te çok büyük bir alan tahsis (malloc) etmiş oluruz. Çok büyük buffer'lar bellek israfına veya kısıtlı sistemlerde belleğin yetmemesine yol açabilir.

**Soru 5: Bonus kısmındaki Çoklu FD (Multiple File Descriptor) olayını nasıl çözdün?**
*Cevap:* Tek bir statik pointer yerine, statik bir pointer dizisi (Array of Pointers) tanımladım: `static char *stash[FD_MAX]`.
İşletim sistemindeki her dosya tanımlayıcısının (fd) atandığı sayısal bir değeri vardır (örneğin 3, 4, 5). Ben `fd`'yi bir indeks olarak kullandım. Dosya numarası 3 ise, sadece `stash[3]` hücresinde işlem yapıldı. Bu sayede farklı dosyaların kalan (buffer) verileri kendi indekslerinde yalıtılmış oldu ve birbirine asla karışmadı.

