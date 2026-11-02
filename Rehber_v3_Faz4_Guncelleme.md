# Faz 4 güncellemesi: Quaternius yerine gerçekçi asset'ler

"Sıfırdan rehber (v3)" Faz 4'ün yeni asset'lere göre düzeltilmiş hali. **4.1 ve 4.2 aynen geçerli**; bu belge 4.3'ün yerine geçer ve 4.4, 4.6, 4.7'de değişen satırları verir. Quaternius paketleri (4.3.1–4.3.7) artık kullanılmıyor; indirdiğin `fantasy_props_megakitstandard.zip`'i import etme.

Faz 0.2 klasör ağacındaki fark: `Content/Quaternius/` yok. Yerine:

```
Content/MedievalVillage/          Rossi paketi (Fab, repoda değil)
Content/Showcase/Props/PolyHaven/ 18 prop (CC0)
Content/Showcase/Props/Sketchfab/ 7 prop (CC-BY), 4.3.2'den sonra 12
```

---

## 4.3 Prop'lar (yeni)

Üç kaynak. Hepsi gerçekçi PBR; birlikte tutarlı görünüyorlar (köyün yanında denendi).

| Kaynak | Lisans | Ne için | Durum |
|---|---|---|---|
| Medieval Village, Marco Maria Rossi (Fab) | Fab Standard: oyunda kullanılır, ham asset repoya girmez | Evler, çit, kuyu, tezgah, araba, fıçı, kazan, meşale, bayrak, çam, çalı, çim, taş, ateş efektleri, ortam sesleri | Projede, `.gitignore`'da |
| Poly Haven | CC0 | Sandık, kasa, sepet, kalkan, kamp ateşi, masa, tabure, fener, küçük dekor | Import edildi |
| Sketchfab | CC-BY 4.0 (yazar adı zorunlu, `CREDITS.md`) | Örs, iksir, parşömen, sikke yığını, ekmek, çuval, çadır | Import edildi; 5 model daha indirilecek (4.3.2) |

### 4.3.1 Proje ayarları (yapıldı, kontrol et)

- Project Settings → Rendering → **Virtual Textures ✓** (Rossi'nin 183 dokusu ve landscape RVT'si buna bağlı). İlk açılışta shader derlemesi uzun sürer.
- Edit → Plugins → **Sun Position ✓** (Rossi'nin gündüz haritaları).
- `Content/MedievalVillage/` ve `SourceArt/` `.gitignore`'da.

### 4.3.2 Eksik beş eşyayı indir

Sketchfab'den (her sayfada Download → glTF, varsa 2K). Zip'leri `Downloads`'a indir; çıkarıp import etme işini script yapıyor (`SourceArt/Sketchfab/import_sketchfab.py`).

| Eşya | Model | Lisans | Not |
|---|---|---|---|
| Sargı bezi | [Worn bandages](https://sketchfab.com/3d-models/4de901157af74589b7e30156797d2e8b) | CC-BY | tarama, 4k tri |
| Ok | [Bow and arrows](https://sketchfab.com/3d-models/d6a00d9c0dca441bb880ba467ecbe373) | CC-BY | yay da gelir, oku ayır |
| Haydut mührü | [Wax seal of the imperial District Starost](https://sketchfab.com/3d-models/ce2c938c6fb64af582d32eddd0aa211b) | **CC0** | müze taraması, 153k tri (Nanite) |
| Odun | [Small wood stack](https://sketchfab.com/3d-models/003b7bc7b771476dafcf24b8b2a2ddaa) | CC-BY | tarama, 267k tri (Nanite); hafif istersen [Woodpile](https://sketchfab.com/3d-models/fcd5d5993be64ca38ef8ae2926574874) (3k) |
| Demir cevheri | [Metal Ore](https://sketchfab.com/3d-models/04f8b82a5e4f41dbb01c3c1493e841b0) | CC-BY | tarama, 4k tri |

Her birinin yazarını `CREDITS.md`'ye ekle (Faz 21.2'deki kredi listesi artık bu dosya).

### 4.3.3 Materyaller (M_PropTrim ve M_PropFoliage yok)

Quaternius'un trim dokuları için yazılan `M_PropTrim` / `M_PropFoliage` **oluşturulmuyor**. Yerine:

| Materyal | Nerede | Kim kullanıyor |
|---|---|---|
| Rossi'nin kendi materyalleri | `MedievalVillage/Materials/` | köy, bitki örtüsü (rüzgar dahil) |
| `M_PolyHaven` (BaseColor, Normal, ARM) | `Showcase/Props/PolyHaven/` | Poly Haven prop'ları |
| `M_PolyHaven_Glass` | aynı yer | fenerin camı |
| `M_Sketchfab` (BaseColor, Normal, Roughness, Metallic, AO) | `Showcase/Props/Sketchfab/` | FBX'ten gelen Sketchfab prop'ları |
| `M_Sketchfab_Masked` | aynı yer | parşömen (opacity) |
| glTF'nin kendi materyalleri | ilgili model klasörü | örs, iksir, çadır |

➕ **EKLE** `M_Sketchfab`'e bir `Tint` parametresi (deri ve cevher varyantları için):

```
[BaseColor.RGB] × [Vector Param "Tint" = FFFFFF] → Base Color
```

Apply, Save. Aynısını `M_PolyHaven`'a da yap (sandık ve kasa varyantlarında işe yarar).

### 4.3.4 Faz 14/18/19'da kullanılacak eşleşmeler

Rehberin sonraki fazlarında geçen Quaternius adlarının karşılıkları. Rossi yolları `/Game/MedievalVillage/Meshes/...`, diğerleri `/Game/Showcase/Props/...`.

| Rehberdeki (Quaternius) | Yeni asset | Kaynak |
|---|---|---|
| `Anvil_Log` | `SM_Anvil` (altlığıyla 122 cm) | Sketchfab |
| `Chest_Wood` (Base + Lid) | `SM_TreasureChest_Bottom` + `SM_TreasureChest_Lid` (+ `_Lock`, `_HandleLeft/Right`) | Poly Haven |
| `Workbench`, `Table_Large` | `SM_WoodenTable02`, `SM_RoundWoodenTable01` | Poly Haven |
| `BookStand` | `SM_RoundWoodenTable01` + üstüne `SM_Scroll` | Poly Haven + Sketchfab |
| `Cauldron` | `Props/BP_Cauldron_001` | Rossi |
| `Barrel`, `Barrel_Apples` | `Props/SM_Barrel_001`, `SM_Barrel_002` | Rossi |
| `Crate_Wooden`, `Crate_Metal`, `Prop_Crate` | `SM_WoodenCrate01`, `SM_WoodenCrate02` | Poly Haven |
| `FarmCrate_Apple` | `SM_WickerBasket01/02` | Poly Haven |
| `Bench`, `Stool` | `Props/SM_Bench_001`, `SM_WoodenStool01` | Rossi, Poly Haven |
| `WeaponStand` | yok: `SM_KiteShield` + `SM_WoodenAxe` masaya dayalı | Poly Haven |
| `Stall_Empty`, `Stall_Cart_Empty`, `Prop_Wagon` | `Props/BP_Stall`, `Blueprints/BP_Cart_01` | Rossi |
| `Lantern_Wall`, `CandleStick_Stand` | `SM_WoodenLantern01` veya `Props/BP_Torch`; `SM_WoodenCandlestick` | Poly Haven, Rossi |
| `Banner_1` | `Blueprints/BP_Flag`, `BP_Flag_Stand_01` | Rossi |
| `Bucket_Wooden_1`, `Rope_1` | `Props/SM_Bucket_001`, `Blueprints/BP_BountingCable_01` | Rossi |
| `Coin_Pile` | `SM_Coins` | Sketchfab |
| `Bonfire` | `SM_StoneFirePit` + `Particles/P_Fire` (veya `BP_Fire_001`) | Poly Haven + Rossi |
| `Tent` | `SM_Tent_01` (bez) + `SM_Tent_02` (ahşap iskelet) | Sketchfab |
| Modüler ev parçaları (`Wall_*`, `Roof_*`, `Door_*`) | `Structure/BP_Wall_*`, `BP_Roof`, `BP_Chimney`, `BP_Stairs` (parametrik) | Rossi |
| `Prop_WoodenFence_*`, kuyu | `Structure/BP_Fence`, `BP_Fence_Gate`, `BP_Well` | Rossi |
| `CommonTree_*`, `Pine_*` | `Scatter/SM_Pine_01/02`, `SMF_Pine_Tall_01` (yalnız çam var) | Rossi |
| `Bush_Common`, `Grass_*`, `Fern_1`, `Plant_1` | `Scatter/SM_Shrub_Pine_00x`, `SM_Grass_Patch_0x`, `SM_Flower_00x` | Rossi |
| `Rock_Medium_*` | `Props/SM_Decorative_Stone_0xx` | Rossi |
| `Dummy` (talim kuklası) | yok; mevcut `BP_TrainingDummy` (Faz 19.1) | - |
| `Mushroom_Common` | yok, kullanma | - |

Rossi'nin evleri tek mesh değil, parametrik blueprint. Hazır kurulu evler `MedievalVillage/Levels/MedievalVillage` haritasında; Faz 19'da oradan seçip Packed Level Actor'e çevireceğiz.

▶ **TEST:** `L_Showcase`'e `SM_Anvil`, bir `SM_Potions_0x`, `Structure/BP_Wall_2mx2m` ve `Scatter/SM_Pine_01` sürükle. Örs metal gibi, iksir camsı, duvar sıvalı, çamın yaprakları delikli (kare değil), çıkıntılar çukur görünmüyor. Sonra dördünü sil.

### 4.3.5 Yerdeki eşya mesh'leri

Yeni prop'lar gerçek boyunda import edildi (Quaternius'taki gibi 100 kat sorunu yok), ama yerde okunması için bazılarını büyütüyoruz. Aynı yöntem: mesh → Details → LOD 0 → Build Settings → **Build Scale** → Apply Changes. Build Scale mutlak bir değer; yeni değer eskisinin yerine geçer.

Dekorda da kullanılan mesh'ler için önce **Duplicate** al, Build Scale'i kopyaya ver.

| Eşya | Mesh | Şu an | Hedef | Build Scale | Not |
|---|---|---|---|---|---|
| Health Potion | `SM_Potions_0x` (kırmızı olanı) | 35 cm (0.1) | 40 cm | **0.114** | 4 renkten kırmızıyı seç |
| Bread | `SM_Bread` | 41 cm (1.0) | 35 cm | **0.85** | - |
| Leather | `SM_BurlapSack` → Duplicate → `SM_Leather_Bundle` | 56 cm | 40 cm | **0.71** | `MI_BurlapSack`'i Duplicate → `MI_Leather`, Tint `7A4A2A` |
| Tarif parşömeni (Faz 8) | `SM_Scroll` | 31 cm (0.33) | 40 cm | **0.43** | - |
| Yedek görünüm | `SM_BurlapSack` → Duplicate → `SM_Pickup_Fallback` | 56 cm | 40 cm | **0.71** | World Mesh'i boş eşyalar bununla görünür |
| Bandage | `SM_Bandage_01` (rulo) | 24 cm | 25 cm | **1.9 (yapıldı)** | `_02` rulo ve `_03` açık sargı gerçek boyda, dekor için |
| Arrow | henüz indirilmedi | - | 75 cm | hesapla | `75 / Approx Size'ın en büyüğü` |
| Bandit Seal | `SM_BanditSeal_02` (mühür, 18 cm) + `SM_BanditSeal_01` (6 cm) | 18 cm | 18 cm | **0.057 (yapıldı)** | taramada iki parça; World Mesh'e mühürün kendisi olanı ver |
| Wood | `SM_Wood` | 70 cm | 70 cm | **0.068 (yapıldı)** | haritadaki odun yığınlarında aktörü büyüt |
| Iron Ore | `SM_IronOre` | 34 cm | 35 cm | 1 (gerek yok) | - |

Ölçeklerin doğruluğunu Faz 4.7 testinde yerde görerek kontrol et.

---

## 4.4 İkon stüdyosu: değişenler

Kurulum (RT_Icon, L_IconStudio, ışıklar, SceneCapture2D) aynı. Gerçekçi asset'ler yüzünden dört ekleme:

1. **Kadraj: ortografik kamera.** Eşyalar artık 18 cm'lik mühürden 122 cm'lik örse kadar gerçek boyda; perspektif kamerada her ikon için mesafe ayarlamak gerekir. SceneCapture2D → Projection → **Projection Type: Orthographic**, **Ortho Width = eşyanın en uzun kenarı × 1.25** (santim). Kamera Location (-500, 0, 0) kalsın; ortografikte mesafe boyutu etkilemez.
2. **Dokuların tam çözünürlüğü.** Virtual Texture ve streaming yüzünden ilk karelerde dokular bulanık gelir. Capture Every Frame ✓ açıkken mesh'i koyduktan sonra 2–3 saniye bekle, sonra Create Static Texture yap. Hâlâ bulanıksa konsola `r.Streaming.FullyLoadUsedTextures 1` yaz.
3. **Işık seviyesi.** Gerçekçi dokular Quaternius'un açık renklerinden koyu. İkonlar sönük görünürse Directional Light Intensity 6 → 10 ve Sky Light 1 → 2 yap. Bütün ikonları aynı ayarla çek.
4. **Saydam materyaller.** İksirin camı translucent ise SceneCapture'ın alfa kanalında görünmeyebilir (ikonda şişe "kesik" durur). Öyleyse iksir için kamerayı hafif yukarıdan, şişenin dolu kısmını gösterecek açıyla çek; ya da iksirin materyal instance'ını Duplicate edip Blend Mode'u Masked yaptığın bir kopyayla çek.

İkon eşleşmesi (4.4 tablosunun yeni hali):

| Doku | Mesh |
|---|---|
| `T_Icon_RustySword`, `_IronSword`, `_SteelSword`, `_DragonBlade` | `SM_Sword_*` (değişmedi, 45° yatık) |
| `T_Icon_LeatherArmor`, `T_Icon_IronHelmet` | paketin `SKM_DKM_Armor`, `SKM_DKM_Helmet` parçaları (değişmedi) |
| `T_Icon_IronOre`, `_Wood`, `_Leather`, `_Arrow` | `SM_IronOre`, `SM_Wood`, `SM_Leather_Bundle`, ok |
| `T_Icon_HealthPotion`, `_Bandage`, `_Bread` | kırmızı `SM_Potions_0x`, `SM_Bandage_01`, `SM_Bread` |
| `T_Icon_BanditSeal` | `SM_BanditSeal_02` |
| `T_Icon_Gold` | `SM_Coins` |

16 ikon, sayı değişmedi.

---

## 4.6 Eşyalar: World Mesh sütunu

Tablonun geri kalanı aynı; yalnızca World Mesh değişiyor:

| Asset | World Mesh (eski → yeni) |
|---|---|
| `DA_Item_IronOre` | `IronOre_Chunk` → `SM_IronOre` |
| `DA_Item_Wood` | `WoodLog` → `SM_Wood` |
| `DA_Item_Leather` | `Leather_Bundle` → `SM_Leather_Bundle` |
| `DA_Item_Arrow` | `Arrow` → ok mesh'i (indirilince; o zamana kadar boş bırak, yedek görünümle durur) |
| `DA_Item_HealthPotion` | `Potion_2` → kırmızı `SM_Potions_0x` |
| `DA_Item_Bandage` | `Bandages` → `SM_Bandage_01` |
| `DA_Item_Bread` | `Bread` → `SM_Bread` |
| `DA_Item_BanditSeal` | `Coin` → `SM_BanditSeal_02` |
| `DA_Item_LeatherArmor`, `DA_Item_IronHelmet` | boş (değişmedi; yedek görünümle durur) |
| Kılıçlar | `SM_Sword_*` (değişmedi) |

## 4.7 BP_ItemPickup: tek değişiklik

Mesh (miras) → Static Mesh = **`SM_Pickup_Fallback`** (eski `Pouch_Large` yerine). Diğer ayarlar ve test aynı.

---

## Sonraki fazlarda etkilenen yerler

Bu fazdaki eşleşme tablosu (4.3.4) Faz 14 (sandık, istasyonlar), Faz 18 (kamp, ışık) ve Faz 19'da (harita) Quaternius adlarının geçtiği her satırın karşılığı. O fazlara gelince ayrıca güncellenecek. Faz 21.2'deki kredi listesi `CREDITS.md`.
