# Faction & Reputation, Dialogue, Save/Load: Kurulum ve Editör Kılavuzu

Bu kılavuz zip'teki üç sistemi projeye yerleştirmekten ilk kayıt/yükleme testine kadar her adımı sırayla anlatıyor.
Sıra önemli. Save, diğer ikisinin (ve önceki sistemlerin) verisini kaydettiği için en sona kalıyor.

> **Kod derlenmeden yazıldı.** Build'de hata çıkarsa log'u olduğu gibi gönder, birlikte düzeltelim.

---

## 0. Ne geliyor? (Özet)

| Plugin | Modüller | Görevi |
|---|---|---|
| **RPGFaction** | RPGFaction, RPGFactionUI | Fraksiyonlar, itibar, rütbeler (Hostile → Honored), fraksiyonlar arası ilişkiler, itibar paneli |
| **RPGDialogue** | RPGDialogue, RPGDialogueUI | Dallanan konuşmalar (data asset), koşullar, aksiyonlar, bayraklar (flags), diyalog ekranı |
| **RPGSave** | RPGSave | Slot'a kaydet/yükle: oyuncunun component'leri + level'e yerleştirilmiş aktörler |

```
                       RPGCore (arayüzler)
     IRPGReputationHolder   IRPGSaveable   IRPGItemContainer   IRPGCurrencyHolder   IRPGTagReceiver
            ▲                    ▲                  ▲                   ▲                  ▲
  Reputation (PlayerState)  her kaydedilen   Inventory (pawn)     Wallet (pawn)      ASC (pawn)
            ▲                 component              ▲                   ▲                  ▲
            └──────── Dialogue koşulları/aksiyonları bunları RPGCore üzerinden kullanıyor ─┘
```

Üç plugin de **sadece RPGCore'a** bağlı. Dialogue "itibar 200'den fazla mı?" sorusunu RPGFaction'ı tanımadan
`IRPGReputationHolder` ile soruyor. Save de hiçbir sistemi tanımıyor: `IRPGSaveable` implement eden her component'in
`UPROPERTY(SaveGame)` alanlarını yazıyor.

| Component | Nereye | Not |
|---|---|---|
| **RPG Reputation** | **PlayerState** | Progression ile aynı yer: ölünce kaybolmuyor |
| **RPG Faction Member** | Düşman, NPC | "Ben Bandits'tenim". Öldürülünce itibarı değiştiriyor |
| **RPG Dialogue** | **PlayerController** | Konuşmayı yürütüyor, bayrakları tutuyor (kaydediliyor) |
| **RPG Dialogue UI** | **PlayerController** | Diyalog ekranını açıp kapatıyor |
| **RPG Dialogue Speaker** | NPC | İsim, portre, konuşma asset'i, olaylar (ticaret aç) |
| **RPG Saveable Actor** | Level'e **yerleştirilmiş** aktörler | Sandık, tüccar, düşman, yerdeki item |

---

## 1. Dosyaları yerleştir

1. Editörü **kapat**.
2. Zip'i **`RPGTemplate/`** kök klasörüne çıkar, üzerine yazmayı onayla.

| Dosya | Durum |
|---|---|
| `Plugins/RPGFaction/` | **Yeni plugin** |
| `Plugins/RPGDialogue/` | **Yeni plugin** |
| `Plugins/RPGSave/` | **Yeni plugin** |
| `RPGCore/.../Interfaces/RPGReputationHolder.h` + `Private/Interfaces/RPGReputationHolder.cpp` | **Yeni** |
| `RPGCore/.../Interfaces/RPGSaveable.h` | **Değişti (tamamen yeni hali)**: `OnBeforeSave()`, `OnLoadedFromSave()` |
| `RPGEconomy/.../RPGEconomyTypes.h` | **Değişti**: stok satırının alanları `SaveGame` |
| `RPGEconomy/.../RPGWalletComponent.h/.cpp` | **Değişti**: `IRPGSaveable` |
| `RPGEconomy/.../RPGMerchantComponent.h/.cpp` | **Değişti**: `IRPGSaveable` |
| `RPGEconomy/.../RPGCurrencyRewardComponent.h/.cpp` | **Değişti**: `bRewardGranted` artık `SaveGame` |

> **Plugin'leri editörün veya Rider'ın "Add Plugin / Add Module" sihirbazıyla OLUŞTURMA.** Zip'tekiler tam plugin.
> Sihirbaz kendi `Build.cs`'ini yazıp bizimkinin üzerine yazıyor (Crafting'de yaşadığımız sorun).

> **`RPGSaveable.h` hakkında:** Projede bu dosyanın eski bir hali vardı. Eski arayüzün başka fonksiyonlarını
> override eden bir sınıfın varsa build "method with override specifier did not override" diyecek. O override'ları sil
> ya da log'u bana gönder.

> **Economy kılavuzundaki 1.1'i** (Inventory'ye `GetItems`) henüz yapmadıysan önce onu yap.

### 1.1 Elle eklenecek kod (önceki sistemler kaydedilsin diye)

Bu dosyalar sende, bende değil. Hepsinde aynı iki adım var:

1. Sınıf `IRPGSaveable`'ı da miras alsın.
2. Kaydedilecek alanlar `UPROPERTY(..., SaveGame, ...)` olsun. `OnLoadedFromSave()` ekranları yenilesin.

> **Önemli kural:** Kayıt sistemi `SaveGame` filtresini **struct'ların içine de** uyguluyor. `TArray<FRPGItemEntry>`
> `SaveGame` olsa bile `FRPGItemEntry`'nin `Item`/`Quantity` alanları `SaveGame` değilse **boş** kaydedilir.
> Inventory dokümanına göre `FRPGItemEntry`'nin üç alanı zaten `SaveGame`, yine de kontrol et.

**A) Inventory (`RPGInventoryComponent.h/.cpp`)**
```cpp
// .h
#include "Interfaces/RPGSaveable.h"

class RPGINVENTORY_API URPGInventoryComponent : public UActorComponent, public IRPGItemContainer, public IRPGSaveable
// public:
	virtual void OnLoadedFromSave() override;

// Stack dizisinin UPROPERTY'sinde SaveGame olduğundan emin ol, örn:
	UPROPERTY(VisibleInstanceOnly, SaveGame, Category = "RPG|Inventory")
	TArray<FRPGItemEntry> Entries;
```
```cpp
// .cpp
void URPGInventoryComponent::OnLoadedFromSave()
{
	OnInventoryRefreshed.Broadcast();
	OnWeightChanged.Broadcast(GetCurrentWeight(), MaxWeight);
}
```
*(`OnWeightChanged` parametreleri sendeki delegate ile aynı olmalı. Ağırlığı cache'lediğin bir alan varsa önce onu yeniden hesapla.)*

**B) Equipment (`RPGEquipmentComponent.h/.cpp`)**
```cpp
// .h: miras listesine  , public IRPGSaveable   + #include "Interfaces/RPGSaveable.h"
	virtual void OnLoadedFromSave() override;
// EquippedItems map'i zaten SaveGame (Equipment.md'ye göre).
```
```cpp
// .cpp
void URPGEquipmentComponent::OnLoadedFromSave()
{
	// SetSlotItem'ın içinde stat snapshot'ını IRPGStatReceiver'a gönderen kısmı çağır (sende adı farklı olabilir)
	PushStatModifiers();

	// Görsel component ve panel her slotu yeniden çizsin. Boş slotlar da gönderiliyor (eski mesh kalmasın).
	for (const FGameplayTag& Slot : AvailableSlots)
	{
		OnEquipmentChanged.Broadcast(Slot, GetEquippedItem(Slot), nullptr);
	}
}
```
*(`GetEquippedItem` yerine sendeki "slottaki item'ı ver" fonksiyonunun adını yaz.)*

**C) Progression (`RPGProgressionComponent`, PlayerState'te)**
```cpp
// .h: , public IRPGSaveable   + include
	virtual void OnLoadedFromSave() override;
// Level, Experience, SkillPoints, UnlockedSkills zaten SaveGame.
```
```cpp
// .cpp
void URPGProgressionComponent::OnLoadedFromSave()
{
	PushToTarget();   // stat + skill tag snapshot'ını pawn'a gönderen fonksiyonun
	OnExperienceChanged.Broadcast(/* sendeki parametreler, örn. Experience, GetExperienceToNextLevel() */);
	OnSkillPointsChanged.Broadcast(SkillPoints);
}
```

**D) Experience Reward (`RPGExperienceRewardComponent`)** ve **E) Loot (`RPGLootComponent`)**

İkisinde de "bir kere" bayrağı var. Kaydedilirse açılmış sandık tekrar loot vermez:
```cpp
// .h: , public IRPGSaveable   + include
private:
	UPROPERTY(SaveGame)
	bool bRewardGranted = false;   // Loot'ta: bool bHasLooted = false;
```
`OnLoadedFromSave` gerekmiyor (varsayılan boş).

**F) Crafting (`RPGCraftingComponent`)**: `KnownRecipes` zaten `SaveGame`. Sadece miras listesine
`, public IRPGSaveable` ve `#include "Interfaces/RPGSaveable.h"` ekle.

### 1.2 `.uproject` ve build

`RPGTemplate.uproject` → `"Plugins"` listesine (diğer RPG plugin'lerinin yanına):
```json
{ "Name": "RPGFaction",  "Enabled": true },
{ "Name": "RPGDialogue", "Enabled": true },
{ "Name": "RPGSave",     "Enabled": true }
```
1. `.uproject` → sağ tık → **Generate Visual Studio project files**.
2. Rider'da build (editör kapalı).

**Kontrol:** Build hatasız. **Edit → Plugins**'te "RPG Faction", "RPG Dialogue", "RPG Save" etkin.

---

## 2. Faction & Reputation

### 2.1 Gameplay Tag'ler

**Project Settings → Gameplay Tags → Manage Gameplay Tags** (+):

| Tag | Ne için |
|---|---|
| `Faction.Village` | Köy |
| `Faction.Bandits` | Haydutlar |
| `Reputation.Village.Hostile`, `.Neutral`, `.Friendly`, `.Honored` | Rütbe tag'leri (pawn'a veriliyor) |
| `Reputation.Bandits.Hostile`, `.Neutral` | |

Rütbe tag'leri pawn'ın ASC'sine **loose tag** olarak gidiyor. Yani bir yetenek `Activation Required Tags =
Reputation.Village.Honored` diyebilir, bir kapı "köyün dostuysan açıl" diye `Has Matching Gameplay Tag` sorabilir.

### 2.2 Fraksiyon asset'leri

Content Browser → sağ tık → **Miscellaneous → Data Asset** → **RPGFactionDefinition**.

**`DA_Faction_Village`**

| Alan | Değer |
|---|---|
| Faction Tag | `Faction.Village` |
| Display Name | **Köy** |
| Description | "Değirmen köyünün halkı." |
| Icon | (varsa bir texture) |
| Starting Reputation | **0** |
| Show In UI | ✓ |
| Ranks | aşağıdaki tablo, **küçükten büyüğe** |
| Relations | `Faction.Bandits`, Multiplier **-0.5** |

| # | Display Name | Min Reputation | Color | Granted Tag | Hostile |
|---|---|---|---|---|---|
| 0 | Düşman | -1000 | (0.8, 0.15, 0.1) kırmızı | `Reputation.Village.Hostile` | ✓ |
| 1 | Nötr | -100 | (0.7, 0.7, 0.7) gri | `Reputation.Village.Neutral` | |
| 2 | Dost | 200 | (0.3, 0.8, 0.3) yeşil | `Reputation.Village.Friendly` | |
| 3 | Onurlu | 600 | (1.0, 0.8, 0.3) altın | `Reputation.Village.Honored` | |

Rütbe = `Min Reputation`'ı geçilen **son** satır. 250 → "Dost", -150 → "Düşman". İlk satırın altına inilirse
yine ilk satır geçerli.

Relations: Köy'e +20 → Haydutlar'a 20 × -0.5 = **-10**. İlişkiler **tek seviye** uygulanıyor (Haydutlar'ın kendi
ilişkileri tekrar tetiklenmiyor, sonsuz döngü yok).

**`DA_Faction_Bandits`**

| Alan | Değer |
|---|---|
| Faction Tag | `Faction.Bandits` |
| Display Name | **Haydutlar** |
| Starting Reputation | **-300** |
| Ranks | 0: "Düşman", -1000, kırmızı, `Reputation.Bandits.Hostile`, Hostile ✓ · 1: "Nötr", 0, gri, `Reputation.Bandits.Neutral` |
| Relations | `Faction.Village`, Multiplier **-0.4** |

Bir haydut öldürünce: Haydutlar -25 → Köy 25 × 0.4 = **+10**.

**Kontrol:** Asset'e sağ tık → **Asset Actions → Validate**. Rütbeler sıralı değilse ya da fraksiyon kendisiyle ilişkiliyse
hata veriyor.

### 2.3 Project Settings

**Project Settings → Game → RPG Factions**

| Alan | Değer |
|---|---|
| Factions | `DA_Faction_Village`, `DA_Faction_Bandits` |
| Min Reputation / Max Reputation | -1000 / 1000 (itibar bu aralığa kırpılıyor) |

> Buraya eklenmeyen fraksiyon **yok sayılır**: başlangıç değeri verilmez, rütbe bulunmaz, panelde çıkmaz.

### 2.4 Oyuncu: `BP_PlayerState`

Progression için zaten bir PlayerState BP'n olmalı (yoksa: Blueprint Class → **PlayerState** → `BP_PlayerState`,
GameMode → **Player State Class** = `BP_PlayerState`).

`BP_PlayerState` → **Add → RPG Reputation**. Ayarı yok.

### 2.5 Düşmanlar

Haydut düşman BP'sine **Add → RPG Faction Member**:

| Alan | Değer |
|---|---|
| Faction | `Faction.Bandits` |
| Kill Reputation Change | **-25** (öldürene Haydutlar itibarı -25, ilişkiler üzerinden Köy +10) |

`RPG Health` → **On Death (Killer)** zincirine ekle (XP ve loot'tan sonra):
```
On Death (Killer)
  → RPG Experience Reward.Grant Reward (Killer)
  → RPG Loot.Drop Loot
  → RPG Faction Member.Report Killed By (Killer)
```
`Report Killed By` ok/mermi gibi durumlarda instigator'a, pawn'dan PlayerState'e kendisi ulaşıyor. Bir düşman için
sadece bir kez çalışıyor.

*(AI için)* `Is Hostile To (Other)` → "Bu oyuncu bizim fraksiyona düşman mı?" (rütbedeki **Hostile** kutusu).
Hedef seçiminde kullanabilirsin.

### 2.6 İtibar paneli

**`WBP_ReputationEntry`** (parent **`RPGReputationEntryWidget`**)
```
SizeBox (Height 56)
└─ Border (0.02, 0.02, 0.02, 0.6), Padding 6
   └─ HorizontalBox
      ├─ SizeBox 40×40 → Image "IconImage"              (opsiyonel)
      └─ VerticalBox (Fill)
         ├─ HorizontalBox
         │  ├─ TextBlock "NameText"   (Fill, 15, açık krem)
         │  └─ TextBlock "RankText"   (14, renk koddan: rütbe rengi)
         ├─ ProgressBar "RankProgressBar"  (Height 8, renk koddan)
         └─ TextBlock "ValueText"     (11, gri)  → "250 / 600"
```

**`WBP_ReputationPanel`** (parent **`RPGReputationPanelWidget`**)
```
SizeBox 420 × Auto → Border (koyu panel, Padding 12) → VerticalBox
├─ TextBlock "İtibar"  (altın, 20 Bold)
└─ VerticalBox "FactionList"
```
Class Defaults → **Entry Class** = `WBP_ReputationEntry`.

Panel sadece bilgi gösteriyor, input modu değiştirmiyor. PlayerController BP'sinde **J** tuşuyla aç/kapa:
```
J (Pressed)
  → IsValid(ReputationPanel)? hayır → Create Widget (WBP_ReputationPanel) → SET ReputationPanel
  → Is In Viewport? → evet: Remove From Parent / hayır: Add To Viewport
```
Panel kendini oyuncunun PlayerState'indeki Reputation'a bağlıyor. Açıkken itibar değişirse anında güncelleniyor.
*(Opsiyonel)* `BP_OnReputationChanged` → "+10 Köy" bildirimi.

### 2.7 Konsol

| Komut | Etki |
|---|---|
| `rpg.Reputation.Add Faction.Village 250` | İtibar ekler (ilişkiler dahil), sonucu log'a yazar |
| `rpg.Reputation.Print` | Tüm fraksiyonlar: değer ve rütbe |

---

## 3. Dialogue

### 3.1 Tag'ler

`Dialogue.Event`, `Dialogue.Event.Trade` ve `Dialogue.Flag` kodda tanımlı (native), elle ekleme. Bayrakları sen ekle:

| Tag | Anlamı |
|---|---|
| `Dialogue.Flag.MetBlacksmith` | Demirciyle tanıştı |
| `Dialogue.Flag.AcceptedBanditJob` | Haydut işini kabul etti |
| `Dialogue.Flag.GotFreeSword` | Bedava kılıcı aldı |

Bayraklar oyuncunun **"hafızası"**: Dialogue component'inde tutuluyor, **kaydediliyor** ve pawn'a gameplay tag olarak da
veriliyor (component'te `Grant Flags As Tags` ✓). Yani bir kapı veya görev de `Dialogue.Flag.AcceptedBanditJob` var mı
diye bakabilir.

### 3.2 PlayerController

GameMode'daki PlayerController BP'sine:

| Component | Ayar |
|---|---|
| **RPG Dialogue** | Max Talk Distance **500**, Grant Flags As Tags ✓ |
| **RPG Dialogue UI** | Screen Widget Class = `WBP_DialogueScreen`, Z Order 30 |

> İkisi de **PlayerController'da**. Karaktere koyarsan "owner is not a local PlayerController" / "has no
> RPGDialogueComponent" uyarısı alırsın.

### 3.3 Widget'lar

**`WBP_DialogueChoice`** (parent **`RPGDialogueChoiceWidget`**)
```
Button "ChoiceButton"     ← Is Focusable ✗, Style: Normal (0,0,0,0), Hovered (1, 0.8, 0.3, 0.15), Padding 6
└─ HorizontalBox
   ├─ TextBlock "NumberText"   (altın, 15, sağ Padding 8)   → "1."
   └─ TextBlock "ChoiceText"   (Fill, 15, Auto Wrap ✓)
```
Renkler Class Defaults'ta: Available (0.95, 0.92, 0.85), Unavailable (0.45, 0.45, 0.45). Koşulu tutmayan cevap gri
ve tıklanamaz.

**`WBP_DialogueScreen`** (parent **`RPGDialogueScreenWidget`**)
```
CanvasPanel
└─ Border (Anchor: alt-orta, Size 1100×300, Position Y -40, Alignment 0.5,1)
   Brush (0.03, 0.025, 0.02, 0.92), Padding 18
   └─ HorizontalBox
      ├─ SizeBox 160×160 → Image "PortraitImage"         (opsiyonel; portre yoksa gizleniyor)
      ├─ Spacer 16
      └─ VerticalBox (Fill)
         ├─ HorizontalBox
         │  ├─ TextBlock "SpeakerNameText"  (Fill, altın (1, 0.8, 0.3), 20 Bold)
         │  └─ Button "CloseButton" → TextBlock "X"     (Is Focusable ✗)
         ├─ TextBlock "DialogueText"  (17, krem (0.95, 0.92, 0.85), Auto Wrap ✓, Padding üst 6 alt 10)
         ├─ VerticalBox "ChoiceList"  (Fill)
         └─ Button "ContinueButton" (sağa hizalı, Is Focusable ✗) → TextBlock "Devam ▸"
```
Class Defaults:

| Alan | Değer |
|---|---|
| Choice Class | `WBP_DialogueChoice` |
| Characters Per Second | **45** (daktilo efekti; 0 = metin bir anda) |
| Close With Escape | ✓ |

Ekran kontrolleri: **1–9** cevap seçer, **Space/Enter** devam eder (yazı akıyorsa önce tamamlar), **Esc** konuşmayı
bitirir, metne tıklamak yazıyı tamamlar. Ekran modal: tıklamalar oyuna (saldırıya) gitmiyor.

### 3.4 Konuşma asset'i: `DA_Dialogue_Blacksmith`

Content Browser → **Data Asset** → **RPGDialogueAsset**.

> Bu sürümde görsel bir graph editörü yok. Konuşma, `Nodes` dizisindeki satırlar ve satırların birbirine
> **Id** ile bağlanmasıyla kuruluyor. Asset'i kaydedince/Validate edince eksik bağlantılar hata olarak çıkıyor.

**Üst alanlar**

| Alan | Değer |
|---|---|
| Start Node | `FirstMeeting` |
| Conditional Starts | [0] Node = `Greeting`, Conditions = **Has Flags** (`Dialogue.Flag.MetBlacksmith`) |

Conditional Starts sırayla kontrol ediliyor, koşulu tutan ilk satır kullanılıyor, hiçbiri tutmazsa Start Node.
Yani ilk konuşmada `FirstMeeting`, sonrakilerde `Greeting`.

**Nodes** (her satır bir replik; `Id` asset içinde benzersiz):

**[0] `FirstMeeting`**
| Alan | Değer |
|---|---|
| Text | "Hoş geldin yabancı. Ben Hakan, köyün demircisi." |
| On Enter Actions | **Set Flags** → Add Flags: `Dialogue.Flag.MetBlacksmith` |
| Choices | boş |
| Next Node | `Menu` |

**[1] `Greeting`**: Text "Yine sen! Ne lazım?", Next Node `Menu`.

**[2] `Menu`**: Text "Ne yapmak istersin?". Choices:

| # | Text | Next Node | Conditions | Actions | Hide If Unavailable |
|---|---|---|---|---|---|
| 0 | "Ne satıyorsun?" | *(boş = konuşma biter)* | | **Send Event**: `Dialogue.Event.Trade`, Wait For Dialogue End ✓ | |
| 1 | "Köy için iş var mı?" | `Job` | **Has Flags** `Dialogue.Flag.AcceptedBanditJob`, **Invert ✓** | | ✓ |
| 2 | "Köye 50 altın bağışlamak istiyorum." | `Donate` | **Has Currency** (boş = Gold) 50 | **Change Currency** -50, **Change Reputation** `Faction.Village` +40 | |
| 3 | "Bana bir kılıç verir misin?" | `FreeSword` | **Reputation At Least** `Faction.Village` 200 · **Has Flags** `Dialogue.Flag.GotFreeSword` Invert ✓ | | |
| 4 | "Hoşça kal." | *(boş)* | | | |

**[3] `Job`**: Text "Haydutlar yolu kesiyor. Birkaçını halledersen köy sana minnettar kalır." Choices:
- "Hallederim." → Next `Menu`, Actions: **Set Flags** Add `Dialogue.Flag.AcceptedBanditJob`
- "Şimdi olmaz." → Next `Menu`

**[4] `Donate`**: Text "Cömertsin! Köy bunu unutmayacak." Next `Menu`.

**[5] `FreeSword`**: Text "Köyün dostusun. Al bakalım, iyi kullan." On Enter Actions: **Change Item** Iron Sword +1,
**Set Flags** Add `Dialogue.Flag.GotFreeSword`. Next `Menu`.

Koşul ve aksiyonlar **instanced**: dizide **+** → açılan listeden türü seç ("Has Flags", "Change Item"...) → alanları
doldur.

**Mevcut koşullar:** Has Flags, Has Tags (pawn'daki gameplay tag'ler: skill, rütbe), Has Item, Has Currency,
Reputation At Least, Blueprint Condition. Hepsinde **Invert** var ("yoksa geç").

**Mevcut aksiyonlar:** Set Flags (ekle/kaldır), Change Item (+ ver / − al), Change Currency (+/−), Change Reputation,
Send Event, Blueprint Action.

> **Blueprint Condition/Action:** Blueprint Class → parent **RPGDialogueCondition_Blueprint** (veya
> `..._Blueprint` action) → `Receive Is Met` / `Receive Execute` override. Context'te Player (pawn), Speaker (NPC),
> Dialogue (component) var.

**Kontrol:** Asset'e sağ tık → **Validate**. Beklenen: hata yok. Bir `Next Node`'a yanlışlıkla `Menuu` yaz →
"points to 'Menuu', which does not exist". Hiçbir yerden gidilemeyen satır uyarı olarak çıkıyor.

### 3.5 NPC: Demirci

Economy'deki `BP_Merchant`'ı kullan (veya kopyala). Ekle: **RPG Dialogue Speaker**

| Alan | Değer |
|---|---|
| Display Name | **Demirci Hakan** |
| Dialogue | `DA_Dialogue_Blacksmith` |
| Portrait | (opsiyonel bir texture) |

`RPG Interactable` → Prompt: **"Talk"**. **On Interacted** zincirini değiştir:
```
On Interacted (Instigator)
  → RPG Dialogue Speaker.Start Dialogue With (Player = Instigator)
```
`RPG Dialogue Speaker` → Events → **On Dialogue Event (Event Tag, Player)** (+):
```
On Dialogue Event (Event Tag, Player)
  → Branch: Event Tag == Dialogue.Event.Trade   (Matches Tag / "==" node)
       True → Open Trade For (Customer = Player, Merchant = RPG Merchant)
```
"Wait For Dialogue End" ✓ olduğu için olay, diyalog ekranı **kapandıktan sonra** geliyor. Ticaret ekranı diyalogun
altında açılmıyor, input modları çakışmıyor.

*(Opsiyonel)* `On Talk Started (Player)` → NPC'yi oyuncuya döndür, `On Talk Ended` → idle animasyonu.

---

## 4. Save / Load

### 4.1 Neler kaydediliyor?

| Ne | Nereden | Şartı |
|---|---|---|
| Oyuncunun component'leri | PlayerController, pawn, PlayerState | `IRPGSaveable` implement etmesi (Bölüm 1.1) + `SaveGame` alanlar |
| BP değişkenleri | Aynı aktörler + kaydedilen dünya aktörleri | Değişkende **SaveGame** kutusu ✓ (Details → Advanced) |
| Level'e yerleştirilmiş aktörler | Sandık, tüccar, düşman, yerdeki item | **RPG Saveable Actor** component'i |
| Yok edilen yerleşik aktörler | Toplanan item, ölen düşman (Destroy oluyorsa) | `Save Destruction` ✓ |
| Oyuncu konumu, kamera yönü, harita | | otomatik |

**Sistem bazında:**

| Sistem | Kaydedilen |
|---|---|
| Inventory | stack'ler |
| Equipment | takılı item'lar (stat'lar yüklemede tekrar uygulanıyor) |
| Progression | level, XP, puanlar, açılan skill'ler |
| Crafting | bilinen tarifler |
| Economy | oyuncu altını, tüccar stoğu, "bu sandık altın verdi" |
| Loot | "bu sandık/düşman loot verdi" |
| Faction | tüm itibarlar (rütbe tag'leri yüklemede tekrar veriliyor) |
| Dialogue | bayraklar |

### 4.2 Dünya aktörleri

Şunlara **Add → RPG Saveable Actor**:

| Aktör | Neden | Ayar |
|---|---|---|
| Sandık BP'si | Loot / altın verildi bilgisi | Save Destruction ✓ |
| `BP_Merchant` | Stok (oyuncunun sattığı item'lar dahil) | ✓ |
| `BP_ItemPickup` (level'e elle koyduklarına) | Toplanınca Destroy oluyor → yüklemede de yok | ✓ |
| Düşman BP'leri | Ölü düşman yüklemede dirilmesin (ölünce Destroy ediliyorsa) | ✓ |
| Fizikli kutu vs. | Konumu | **Save Transform ✓** |

> Sadece **level'e editörde yerleştirilmiş** aktörler kaydediliyor. Oyun sırasında **spawn** edilenler (loot'tan düşen
> pickup'lar, sonradan doğan düşmanlar) kaydedilmiyor: isimleri her oyunda farklı, geri bulunamaz.

**Sandık görseli:** Sandık BP'sinde `bIsOpened` gibi bir değişken varsa → Details → Advanced → **SaveGame ✓**. Sonra
`RPG Saveable Actor` → **On Loaded From Save** (+) → `bIsOpened` ise kapağı açık pozisyona al.

### 4.3 Tuşlar

PlayerController BP'si:
```
F5 (Pressed) → Get Game Instance Subsystem (RPGSaveSubsystem) → Save To Slot (Slot Name = "Quick")
F9 (Pressed) → Get Game Instance Subsystem (RPGSaveSubsystem) → Load From Slot (Slot Name = "Quick")
```
*(Opsiyonel)* Subsystem'in **On Saved (Slot, Success)** olayına bağlanıp ekrana "Oyun kaydedildi" yaz. Bağlamayı
PlayerController BeginPlay'de yap. Yükleme haritayı yeniden açtığı için **On Loaded** dinleyicisi de yeni dünyada
olmalı (GameInstance BP'sinde dinlemek en sağlamı).

Konsol: `rpg.Save`, `rpg.Load`, `rpg.Save Slot2`, `rpg.Load Slot2`.

Kayıt dosyası: `RPGTemplate/Saved/SaveGames/Quick.sav`.

### 4.4 Yükleme nasıl çalışıyor?

```
Load From Slot("Quick")
  1. Dosyayı oku, sürümü kontrol et
  2. Kaydedilen haritayı YENİDEN AÇ  (kayıttan sonra olan her şey sıfırlanır)
  3. Bütün BeginPlay'ler çalışır (başlangıç item'ları, başlangıç altını vs. — birazdan üzerine yazılacak)
  4. Bir tick sonra:
       - kayıtta "yok edildi" olan yerleşik aktörleri yok et
       - oyuncuyu kayıttaki yere ışınla
       - her component'in SaveGame alanlarını geri yükle
       - HEPSİ bittikten sonra her component'e OnLoadedFromSave → UI'lar yenilenir, stat/tag'ler tekrar uygulanır
  5. On Loaded (Slot, true)
```

---

## 5. Testler

### 5.1 Faction

| # | Adım | Beklenen |
|---|---|---|
| F1 | Oyunu başlat, **J** | Panel: Köy "Nötr" 0, Haydutlar "Düşman" -300 |
| F2 | `rpg.Reputation.Add Faction.Village 250` | Köy 250 "Dost" (yeşil), Haydutlar -425 (-300 + 250 × -0.5). Panel açıksa anında güncellenir |
| F3 | `showdebug abilitysystem` (veya ASC tag listesi) | Pawn'da `Reputation.Village.Friendly` var, `.Neutral` yok |
| F4 | Bir haydut öldür | Haydutlar -25, Köy +10. `rpg.Reputation.Print` ile kontrol |
| F5 | `rpg.Reputation.Add Faction.Village 5000` | 1000'de durur (Max Reputation), "Onurlu" |
| F6 | `rpg.Reputation.Add Faction.Village -5000` | -1000, "Düşman", pawn'da `Reputation.Village.Hostile` |
| F7 | Ölüp yeniden doğ (respawn varsa) | Yeni pawn'da rütbe tag'leri tekrar var |

### 5.2 Dialogue

Başlangıç: Köy 0, 100 Gold.

| # | Adım | Beklenen |
|---|---|---|
| D1 | Demirciye bak | "Talk" prompt'u |
| D2 | **E** | Ekran açılır, isim "Demirci Hakan", metin harf harf akar, imleç görünür, karakter dönmüyor/yürümüyor |
| D3 | Yazı akarken **Space** | Metin anında tamamlanır |
| D4 | **Devam** | `Menu`: 5 cevap. "Bana bir kılıç..." **gri** (itibar yetmiyor) |
| D5 | Gri cevaba tıkla / **4**'e bas | Hiçbir şey olmuyor |
| D6 | **2** (iş) → "Hallederim." | Menüye dönülür, "Köy için iş var mı?" artık **görünmüyor** (Hide ✓) |
| D7 | **3** (bağış) | Altın 50, Köy +40 (panel/Print), "Cömertsin!" |
| D8 | Bağışı tekrar dene | Altın 50 → bağış yapılır, altın 0. Üçüncü denemede cevap **gri** (Has Currency) |
| D9 | **Esc** | Ekran kapanır, kamera/hareket geri gelir |
| D10 | Tekrar konuş | Bu sefer "Yine sen! Ne lazım?" (Conditional Start + MetBlacksmith bayrağı) |
| D11 | "Ne satıyorsun?" | Diyalog kapanır, **hemen ardından** ticaret ekranı açılır |
| D12 | `rpg.Reputation.Add Faction.Village 200` → konuş → kılıç | Iron Sword envantere gelir, "+1 Iron Sword" bildirimi. Cevap tekrar gri (GotFreeSword) |
| D13 | Konuşurken uzaklaş (ekran açıkken hareket kapalı; test için Max Talk Distance'ı 50 yap) | Konuşma kendiliğinden biter. Sonra 500'e geri al |
| D14 | Asset'te bir Next Node'u bozuk yaz → Validate | Hata mesajı. Oyunda o seçenek konuşmayı bitirir, log'da uyarı |

### 5.3 Save / Load

Hazırlık: Sandık ve tüccara **RPG Saveable Actor**, level'e elle konmuş 1 pickup (o da Saveable).

| # | Adım | Beklenen |
|---|---|---|
| S1 | Birkaç item topla, 1 item kuşan, biraz XP/level al, tüccara bir şey sat, demirciyle konuş (bayraklar), itibar değiştir | — |
| S2 | **F5** | Log: `Save 'Quick': OK (N objects, ...)`. `Saved/SaveGames/Quick.sav` oluştu |
| S3 | Değişiklik yap: item at, altın harca, sandığı aç, yerdeki pickup'ı al, başka yere yürü | — |
| S4 | **F9** | Harita yeniden açılır. Log: `Load 'Quick': restored ...` |
| S5 | Kontrol: konum | S2'deki yer ve bakış yönü |
| S6 | Kontrol: envanter, ağırlık, kuşanılmış item (mesh + stat) | S2'deki gibi |
| S7 | Kontrol: level, XP barı, skill puanları, açılmış skill'ler (skill tree) | S2'deki gibi |
| S8 | Kontrol: altın (HUD), itibar (panel), tarifler (crafting) | S2'deki gibi |
| S9 | Kontrol: sandık | S2'den önce açılmadıysa tekrar açılabilir; S2'den önce açıldıysa loot vermez |
| S10 | Kontrol: yerdeki pickup | S2'den sonra alındıysa geri gelmiş; önce alındıysa yok |
| S11 | Kontrol: tüccar stoğu | Sattığın item S2'deki gibi stokta |
| S12 | Demirciyle konuş | "Yine sen!" (bayraklar geri geldi) |
| S13 | Kayıt yokken `rpg.Load Yok` | Log: `no such save.`, hiçbir şey olmuyor |
| S14 | Düşman öldür → F5 → F9 | Düşman Destroy ediliyorsa ölü kalır (ragdoll bırakıyorsan aktör yaşıyor, dirilir; Bkz. Kısıtlamalar) |

Hepsi geçince `Faction.md`, `Dialogue.md`, `Save.md` ve README'yi yazıp commit atacağız.

---

## 6. Sık karşılaşılabilecek sorunlar

| Belirti | Sebep | Çözüm |
|---|---|---|
| Build: `did not override any base class methods` (Saveable ile ilgili) | Eski `RPGSaveable.h`'yi kullanan sınıf | Bölüm 1, eski override'ları sil |
| Build: `cannot open include file RPGDialogueTypes.h` vb. | Plugin sihirbazı Build.cs'in üzerine yazdı | Zip'teki Build.cs'leri tekrar kopyala |
| Build: `IMPLEMENT_MODULE` iki kez | Sihirbazın oluşturduğu fazladan modül .cpp'si | Fazla dosyayı sil |
| Panel boş | Fraksiyonlar Project Settings'e eklenmedi ya da `Show In UI` ✗ | 2.3 |
| İtibar değişmiyor, log: "No RPGReputationComponent" | Reputation PlayerState'te değil / GameMode PlayerState sınıfı yanlış | 2.4 |
| Rütbe tag'i pawn'da yok | Pawn'ın ASC'si `IRPGTagReceiver` değil, ya da rütbede Granted Tag boş | Progression skill tag'leri çalışıyorsa bu da çalışmalı |
| **E** → diyalog açılmıyor, log: "has no RPGDialogueComponent on its PlayerController" | Component karakterde | 3.2 |
| Log: "ScreenWidgetClass is not set" | Dialogue UI'da widget seçilmemiş | 3.2 |
| Log: "has no node 'X'" | Start Node yanlış | Asset'i Validate et |
| Cevaplar görünmüyor, log: "ChoiceClass is not set" | Screen'in Class Defaults'u | 3.3 |
| Rakam tuşları çalışmıyor | Butonlardan biri odağı aldı | Bütün butonlarda **Is Focusable ✗** |
| Ticaret ekranı açılmıyor | Branch'te tag karşılaştırması yanlış ya da `Open Trade For`'a Merchant bağlanmadı | 3.5 |
| Yükleme sonrası envanter boş | `FRPGItemEntry` alanları `SaveGame` değil | 1.1 "Önemli kural" |
| Yükleme sonrası item var ama UI eski | `OnLoadedFromSave` eklenmedi / broadcast yok | 1.1 |
| Yükleme sonrası stat'lar yanlış | Equipment/Progression `OnLoadedFromSave` push çağırmıyor | 1.1 B, C |
| Toplanan pickup yüklemede geri geliyor | Pickup'ta Saveable Actor yok veya pickup spawn edilmiş (level'de değil) | 4.2 |
| Log: "a load is in progress" | Yükleme bitmeden kaydetmeye çalışıldı | Bir saniye bekle |

---

## 7. Bilinen kısıtlamalar (bu sürüm)

| Kısıtlama | Not |
|---|---|
| Diyalog için görsel graph editörü yok | Node'lar dizide, Id ile bağlanıyor. Editör modülü sonraya bırakıldı |
| Ekranlar üst üste açılmıyor | Diyalog açıkken ticaret açılmıyor; bu yüzden olaylar diyalog bitince gönderiliyor |
| Spawn edilen aktörler kaydedilmiyor | Yere düşen loot, sonradan doğan düşmanlar |
| Can/mana gibi GAS attribute'ları kaydedilmiyor | Yükleme tam canla başlıyor. Stat'lar (MaxHealth vs.) ekipman/progression'dan yeniden hesaplanıyor |
| Tek harita | Kayıt haritanın adını tutuyor ve onu açıyor. Haritalar arası kalıcı dünya durumu yok |
| Tek oyunculu | Hiçbiri replike edilmiyor |
| Yükleme haritayı yeniden açıyor | Kısa bir yükleme anı oluyor. Aynı haritada "yerinde" yükleme yok (kayıttan sonra olanları geri almanın tek temiz yolu bu) |
