/**
 * Script to extract the Rival Realms images from "Interfac.dat".
 */

#include "pch.h"
#include "interface-extractor.h"

#include <cstdint>
#include <iostream>
#include <vector>

namespace InterfaceExtractor {

    // NOTE: It is possible these offsets will differ for other locales
    const int OFFSET_TITLE = 0x2131;
    const int OFFSET_HIRE_TROOP_ICONS = 0x1938DBB;

    // Number of colours in the palette
    const int PALETTE_SIZE = 256;

    // The palette used in-game.
    // The last colour (index 0xff) is used for transparent areas.
    /* clang-format off */
    const uint32_t PALETTE_GAME[PALETTE_SIZE] = {
        0x000000ff, 0xccb78fff, 0xa4a494ff, 0x8c846cff,    0x9c845cff, 0x9c7c54ff, 0x94744cff, 0x8c7454ff,
        0x846c54ff, 0x7b6747ff, 0x74644cff, 0x6c6454ff,    0xeacf09ff, 0xf0a705ff, 0xfe7f31ff, 0xfe5027ff,
        0xd10404ff, 0x9d1a1aff, 0x645c4cff, 0x6c5c44ff,    0x64543cff, 0x5c543cff, 0x545444ff, 0x4c5444ff,
        0x4c4c3cff, 0x544c3cff, 0x544c34ff, 0x5c4c34ff,    0x644c2cff, 0x64542cff, 0x6c5434ff, 0x745c34ff,
        0x7c542cff, 0x84542cff, 0x8c5c2cff, 0x8c5424ff,    0x9c6434ff, 0xa46c3cff, 0xb4743cff, 0xbc742cff,
        0xc47c34ff, 0xbc844cff, 0xbc8c54ff, 0xb48454ff,    0xac7c4cff, 0xcc8c4cff, 0xe88f37ff, 0xf49c54ff,
        0xcc7414ff, 0xdc6c04ff, 0xbc640cff, 0xac5c0cff,    0xac6c2cff, 0x8c6c44ff, 0x846444ff, 0x7c5c44ff,
        0x6c4c2cff, 0x644424ff, 0x5c442cff, 0x54442cff,    0x4c3c2cff, 0x443c2cff, 0x3c3c2cff, 0x3c3c34ff,
        0x343c34ff, 0x2c3434ff, 0x34342cff, 0x3d2e2eff,    0x3c2c24ff, 0x3c3424ff, 0x443424ff, 0x4c341cff,
        0x5c3c24ff, 0x643c1cff, 0x5c5a20ff, 0x444424ff,    0x444434ff, 0x24342cff, 0x242c2cff, 0x2c2424ff,
        0x2c2c1cff, 0x34241cff, 0x1c1c1cff, 0x14140cff,    0x0c0c0cff, 0x060605ff, 0x707522ff, 0x849324ff,
        0x94ac24ff, 0xa7c921ff, 0xb4dc24ff, 0xd4fc2cff,    0x041404ff, 0x0c1c04ff, 0x101104ff, 0x142c04ff,
        0x1c3404ff, 0x143414ff, 0x143c14ff, 0x144414ff,    0x144c14ff, 0x145414ff, 0x145c14ff, 0x0c4c0cff,
        0x0c440cff, 0x0c3c0cff, 0x04540cff, 0x2c4c0cff,    0x2c440cff, 0x344c0cff, 0x34540cff, 0x44640cff,
        0x4c740cff, 0x547c14ff, 0x548414ff, 0x5c9414ff,    0x649c14ff, 0x6cb41cff, 0x7cd41cff, 0x7c8444ff,
        0x6c6c3cff, 0x8c0000ff, 0x5b2b10ff, 0x201911ff,    0x2c1c14ff, 0x361f07ff, 0x3f2910ff, 0x463415ff,
        0xfbee9aff, 0xeecc81ff, 0xd3c7a5ff, 0xcfcab4ff,    0xc9baa3ff, 0xb7b099ff, 0xaca48cff, 0x9a9897ff,
        0x88949bff, 0x8c8c8cff, 0x7d848aff, 0x76716dff,    0x747c65ff, 0x766a57ff, 0x81693aff, 0x946c2cff,
        0x978872ff, 0x9f947aff, 0xbe9861ff, 0xeab456ff,    0xfc641cff, 0x666b72ff, 0xa4a4a4ff, 0xb4b4b4ff,
        0xb9bdc1ff, 0xccd0cfff, 0xe4d4c4ff, 0xc0321bff,    0x7d4e22ff, 0x605434ff, 0x595856ff, 0x2c1c2cff,
        0xff4155ff, 0xc7000eff, 0x88000dff, 0x68000aff,    0x500008ff, 0x380006ff, 0x44c1fcff, 0x0680c1ff,
        0x045b84ff, 0x044564ff, 0x04354cff, 0x042534ff,    0x41ff96ff, 0x00c760ff, 0x00883eff, 0x006830ff,
        0x005024ff, 0x003819ff, 0xffdd41ff, 0xc7aa00ff,    0x887100ff, 0x685600ff, 0x504200ff, 0x382e00ff,
        0xff8841ff, 0xc75100ff, 0x883400ff, 0x682800ff,    0x501e00ff, 0x381500ff, 0xff41c1ff, 0xc7007fff,
        0x88005aff, 0x680045ff, 0x500036ff, 0x380026ff,    0x4241ffff, 0x0700c7ff, 0x010088ff, 0x010068ff,
        0x000050ff, 0x000038ff, 0xa2a2a2ff, 0x616161ff,    0x3e3e3eff, 0x2b2b2bff, 0x1c1c1cff, 0x0b0b0bff,
        0xffffffff, 0xb1b1b1ff, 0x808080ff, 0x626262ff,    0x484848ff, 0x252525ff, 0x843c0cff, 0xd46414ff,
        0xfc9424ff, 0xfca424ff, 0xfcc434ff, 0xffe432ff,    0xfcfc3cff, 0xfcf46cff, 0xfcfc9cff, 0xe4c44cff,
        0x050583ff, 0x06066eff, 0x0202c4ff, 0x0202a0ff,    0x196c97ff, 0x115585ff, 0x10516dff, 0x1c80dcff,
        0x1770acff, 0x2476d1ff, 0x255698ff, 0x134572ff,    0x57350cff, 0x3e280dff, 0x31230cff, 0x040c3cff,
        0x0c1c64ff, 0x2c3cacff, 0x0c4cccff, 0x3c4cecff,    0x4c5ce4ff, 0x5c6cd4ff, 0x844cc4ff, 0x5414f4ff,
        0x1c84e4ff, 0x3474a4ff, 0x1c741cff, 0x1c9c1cff,    0x34d434ff, 0x44fc44ff, 0xfca4acff, 0xffffff00
    };

    // The palette used in-the title screen.
    // (no transparency)
    const uint32_t PALETTE_TITLE[PALETTE_SIZE] = {
        0x000000ff, 0x313131ff, 0x5a5a5aff, 0x636363ff,    0x7b7b7bff, 0xa5a5a5ff, 0xb5b5b5ff, 0x100000ff,
        0x210000ff, 0xbd0000ff, 0xc60000ff, 0xd60000ff,    0xff0000ff, 0x5a0800ff, 0x7b1000ff, 0xf72100ff,
        0xbd3921ff, 0xad3921ff, 0x9c5a4aff, 0xbd5239ff,    0xde5231ff, 0xbd3918ff, 0xad3110ff, 0xad2100ff,
        0x9c4229ff, 0x7b2910ff, 0x9c2100ff, 0xbd2900ff,    0x634a42ff, 0xf77b52ff, 0x4a1808ff, 0x631800ff,
        0xad2900ff, 0xc63100ff, 0xde3900ff, 0xce7352ff,    0xbd5229ff, 0x391000ff, 0x7b2100ff, 0x9c2900ff,
        0xb53100ff, 0xbd3100ff, 0xce3900ff, 0xbd3900ff,    0xc63900ff, 0x7b5a4aff, 0x9c3100ff, 0xd69473ff,
        0x945231ff, 0xe77339ff, 0x422110ff, 0xbd5218ff,    0xd64a00ff, 0xd65a10ff, 0x421800ff, 0xc64a00ff,
        0xe75200ff, 0x4a3121ff, 0xce7339ff, 0x9c5221ff,    0x633110ff, 0x7b3910ff, 0xbd5210ff, 0xf7a56bff,
        0xef9452ff, 0x391800ff, 0xffb57bff, 0x634229ff,    0x4a2910ff, 0x9c5218ff, 0x944a10ff, 0xffcea5ff,
        0x945a29ff, 0x7b4a21ff, 0xe77b21ff, 0x7b3900ff,    0xa59c94ff, 0xb59c84ff, 0xffb573ff, 0xce8442ff,
        0xffa552ff, 0xf79c4aff, 0x291808ff, 0xce6b10ff,    0xe77308ff, 0x9c4a00ff, 0x948c84ff, 0xffc68cff,
        0xd6a573ff, 0x181008ff, 0x7b5229ff, 0xbd7b39ff,    0xef9439ff, 0x945a21ff, 0x7b4a18ff, 0x4a2908ff,
        0x6b635aff, 0x292118ff, 0xb57329ff, 0xce7b21ff,    0xb56b18ff, 0xb56308ff, 0xbd8c52ff, 0xb5844aff,
        0x7b5a31ff, 0xef9c39ff, 0xbd7318ff, 0x523108ff,    0x4a2900ff, 0xefd6b5ff, 0xad9473ff, 0xf7a539ff,
        0xef9c31ff, 0xf79408ff, 0xffce84ff, 0x7b5a29ff,    0xffb54aff, 0x735221ff, 0xc68c39ff, 0x634210ff,
        0x5a3908ff, 0xa56300ff, 0xd6ad6bff, 0xc69c5aff,    0xbd9452ff, 0xffbd52ff, 0xbd8c39ff, 0xcea55aff,
        0xdea542ff, 0xf7b539ff, 0xce9429ff, 0xded6c6ff,    0xc6b594ff, 0x9c8452ff, 0x635231ff, 0xefc673ff,
        0xffc652ff, 0xf7bd4aff, 0xad8431ff, 0x423110ff,    0x634a18ff, 0xffb521ff, 0xd69408ff, 0xf7efdeff,
        0xefe7d6ff, 0xc6a55aff, 0xc69c42ff, 0xb58c31ff,    0xffc642ff, 0xe7ad31ff, 0x735208ff, 0xcebd94ff,
        0xb5a57bff, 0xad9c73ff, 0x736339ff, 0x6b5a31ff,    0x635229ff, 0xb59442ff, 0x7b6329ff, 0xad8c39ff,
        0x524218ff, 0x947321ff, 0xf7bd29ff, 0x634a08ff,    0xffb500ff, 0xe7d6a5ff, 0xad9c6bff, 0x8c7b4aff,
        0xceb56bff, 0x423921ff, 0xc6a542ff, 0xbd9c39ff,    0xffce4aff, 0xb59431ff, 0x7b6321ff, 0xad8c29ff,
        0xa58421ff, 0xffc629ff, 0x9c7b18ff, 0xe7ad00ff,    0xded6bdff, 0xf7e7b5ff, 0xd6c694ff, 0xefdea5ff,
        0x9c8c5aff, 0xc6ad5aff, 0x847339ff, 0x9c8439ff,    0xffd652ff, 0xb59429ff, 0x7b6318ff, 0xf7bd00ff,
        0xc6bd9cff, 0xf7e7adff, 0xdece94ff, 0xbdad73ff,    0xe7ce73ff, 0xbda552ff, 0x8c7b39ff, 0xffde63ff,
        0x847331ff, 0xffd642ff, 0xbd9c21ff, 0xffc600ff,    0xd6ceadff, 0x524a29ff, 0x948442ff, 0xdec65aff,
        0xd6bd52ff, 0xceb54aff, 0xc6ad42ff, 0x423910ff,    0xcead29ff, 0xffd621ff, 0xefde8cff, 0xd6c67bff,
        0xe7d684ff, 0xe7d67bff, 0xdec652ff, 0xffd600ff,    0x847b42ff, 0xdec642ff, 0xd6bd21ff, 0xf7efb5ff,
        0x948c52ff, 0xdece63ff, 0xe7d663ff, 0xffe74aff,    0xc6ad00ff, 0xd6bd00ff, 0xefe79cff, 0xffef52ff,
        0xc6b521ff, 0xfff700ff, 0xf7f7efff, 0xdedeceff,    0x181810ff, 0x313118ff, 0xbdc6c6ff, 0x424a4aff,
        0x394242ff, 0xa5bdc6ff, 0x73848cff, 0x394a52ff,    0xc6d6deff, 0x7394a5ff, 0x5a636bff, 0xc6ced6ff,
        0xa5adb5ff, 0x8c949cff, 0x6b737bff, 0x4a525aff,    0x212931ff, 0xdedee7ff, 0x6b6b73ff, 0xffffffff
    };

    // The palette used in-the loading screen.
    // The first colour (index 0x00) is used for transparent areas.
    const uint32_t PALETTE_LOADING[PALETTE_SIZE] = {
        0x00000000, 0x8c2921ff, 0x733118ff, 0xff6329ff,    0xff6b31ff, 0x843110ff, 0xad3908ff, 0x943910ff,
        0xe75a18ff, 0xc64208ff, 0xd64a08ff, 0xad5a31ff,    0x633118ff, 0xff7329ff, 0x943908ff, 0x8c3100ff,
        0x8c5231ff, 0xf77b31ff, 0xce5a18ff, 0xa54208ff,    0xc66b31ff, 0x633110ff, 0x733108ff, 0x944a18ff,
        0x733910ff, 0x8c4210ff, 0xce5a08ff, 0xb56b31ff,    0xa55a21ff, 0x8c4a18ff, 0x9c5218ff, 0x844210ff,
        0xe77318ff, 0x843900ff, 0x9c6b42ff, 0x8c5a31ff,    0xbd7b42ff, 0x6b4221ff, 0xad6b31ff, 0xa56329ff,
        0xd67b31ff, 0x9c5a21ff, 0x734218ff, 0x844a18ff,    0xad5208ff, 0x211810ff, 0xad845aff, 0x634a31ff,
        0x8c6339ff, 0xa57342ff, 0xbd844aff, 0xb57b42ff,    0x7b5229ff, 0x312110ff, 0xa56b31ff, 0x8c5a29ff,
        0x734a21ff, 0x945a21ff, 0xf79431ff, 0x8c5218ff,    0x633910ff, 0x4a2908ff, 0xc66b10ff, 0xa57b4aff,
        0xad7b42ff, 0x845a29ff, 0xa56b29ff, 0xad8c63ff,    0x94734aff, 0xbd8c52ff, 0xe7ad63ff, 0x9c7342ff,
        0xb5844aff, 0x7b5a31ff, 0xe7a552ff, 0x9c6b31ff,    0xd69442ff, 0x946329ff, 0x6b5a42ff, 0x7b6342ff,
        0xefbd7bff, 0x6b5231ff, 0xefb563ff, 0x523918ff,    0xe79c39ff, 0xde9431ff, 0xb59463ff, 0xdead63ff,
        0xdeceb5ff, 0xce8c21ff, 0xffad21ff, 0xc68418ff,    0xe79410ff, 0xdec69cff, 0x9c845aff, 0xffa500ff,
        0xd6bd8cff, 0x847352ff, 0xbda573ff, 0x9c8452ff,    0xcea552ff, 0xefbd52ff, 0xad9c73ff, 0xceb573ff,
        0xf7ce63ff, 0xefbd39ff, 0x736b52ff, 0x6b634aff,    0x4a4229ff, 0xd6bd6bff, 0xbd9410ff, 0xc6b573ff,
        0xffe78cff, 0xf7de84ff, 0xefd67bff, 0xbdad6bff,    0x524a29ff, 0xbdb58cff, 0x948c63ff, 0xefde8cff,
        0xd6c673ff, 0xffd600ff, 0xa59c63ff, 0xffef8cff,    0xefd64aff, 0x948421ff, 0xf7d600ff, 0x6b6329ff,
        0x948410ff, 0xd6bd10ff, 0xceb508ff, 0xd6bd08ff,    0xffde00ff, 0xb5ad6bff, 0xada563ff, 0xbdb56bff,
        0xada55aff, 0x6b6321ff, 0xbdad10ff, 0xfff79cff,    0xd6ce6bff, 0xada542ff, 0xbdb542ff, 0xa59c18ff,
        0xb5ad21ff, 0xffffefff, 0x424239ff, 0xffffceff,    0xf7f7bdff, 0x424231ff, 0xcece8cff, 0xffffadff,
        0x73734aff, 0x8c8c5aff, 0x848452ff, 0x94945aff,    0x9c9c5aff, 0x94944aff, 0xbdbd52ff, 0xc6c642ff,
        0x7b7b21ff, 0xadb57bff, 0x6b7342ff, 0x525a29ff,    0x7b8452ff, 0xbdc69cff, 0x525a39ff, 0x73844aff,
        0x4a5239ff, 0x526b29ff, 0x8c9c73ff, 0x4a5a31ff,    0xc6e79cff, 0x5a9410ff, 0x313929ff, 0x94ad7bff,
        0xadce8cff, 0x293121ff, 0xb5de8cff, 0x394a29ff,    0x4a6b31ff, 0x527b31ff, 0x7b9c63ff, 0x395a21ff,
        0x526b42ff, 0x527b39ff, 0x5a9439ff, 0xbdceb5ff,    0x396b21ff, 0x426b31ff, 0x5a9442ff, 0x528c39ff,
        0xa5bd9cff, 0x425a39ff, 0x527b42ff, 0x5a9c42ff,    0x395231ff, 0x4a7b39ff, 0x5aa542ff, 0x639c52ff,
        0x427b31ff, 0x73ad63ff, 0x528c42ff, 0x4a8c39ff,    0x395a31ff, 0x294a21ff, 0x318c21ff, 0x5a8c52ff,
        0x215a18ff, 0x4a8c42ff, 0x216b18ff, 0x4aa542ff,    0x297b21ff, 0x6b846bff, 0x101810ff, 0x314a31ff,
        0x183118ff, 0x184a18ff, 0x00f718ff, 0x94b59cff,    0x84a594ff, 0x7b9c8cff, 0x7ba594ff, 0x8cb5a5ff,
        0x4a635aff, 0x294239ff, 0x5a7b73ff, 0x739c94ff,    0x84b5adff, 0x739c9cff, 0x527373ff, 0x395252ff,
        0x639494ff, 0x396363ff, 0x52848cff, 0x6b9ca5ff,    0x4a737bff, 0x4a6b73ff, 0x6ba5b5ff, 0x639cadff,
        0x29424aff, 0x183139ff, 0x102931ff, 0x426b7bff,    0x294a5aff, 0x5a8ca5ff, 0x21394aff, 0x183142ff,
        0x4a8cbdff, 0x396b94ff, 0x21527bff, 0x3973adff,    0x295a8cff, 0x21426bff, 0x3163a5ff, 0xffffffff
    };

    // The palette used in-the menus.
    // The first colour (index 0x00) is used for transparent areas.
    const uint32_t PALETTE_MENU[PALETTE_SIZE] = {
        0x00000000, 0x0c0c04ff, 0x381b1cff, 0x78595aff,    0x672930ff, 0x67484bff, 0x58272dff, 0x7a303bff,
        0x934c57ff, 0x804e56ff, 0xa26b73ff, 0x491c24ff,    0x382024ff, 0x75454dff, 0x935d67ff, 0x340c14ff,
        0x4a2b34ff, 0x250c14ff, 0x5b5255ff, 0xe2dddfff,    0x362c34ff, 0x150c14ff, 0x0c040cff, 0x151415ff,
        0x04040cff, 0x0b0c14ff, 0x191c24ff, 0x5e6474ff,    0x454c54ff, 0x555c64ff, 0x656c74ff, 0x4c545cff,
        0x5c646cff, 0x252d34ff, 0x323c44ff, 0x727e87ff,    0xa7adb1ff, 0x1b313fff, 0x3a5464ff, 0x425c6cff,
        0x3a4b56ff, 0x3a454cff, 0x294f64ff, 0x385e73ff,    0x2b4656ff, 0x293f4bff, 0x4a6676ff, 0x44545dff,
        0x5a6c76ff, 0x54646dff, 0x7f888dff, 0x8e979cff,    0x5f7784ff, 0x4b5c65ff, 0x697479ff, 0x2b3539ff,
        0x555a5cff, 0x656a6cff, 0x3c545cff, 0x4b646cff,    0x1b2427ff, 0x76797aff, 0x464b4cff, 0x0c1415ff,
        0x3b4344ff, 0x040c0cff, 0x546c6cff, 0x445454ff,    0x4c5c5cff, 0x546464ff, 0x5c6c6cff, 0x4e5454ff,
        0x5e6464ff, 0x040c04ff, 0x0c140cff, 0x2c3429ff,    0x4c5449ff, 0x686c5eff, 0x606157ff, 0x2c2c1cff,
        0x46463bff, 0x36351cff, 0x77766aff, 0x25241bff,    0x6f6e66ff, 0x37352bff, 0x342c0cff, 0xf4d44cff,
        0xfcdb54ff, 0xf4d454ff, 0xeac74aff, 0xe8c855ff,    0xf4d45cff, 0x595547ff, 0x2c240bff, 0xdeb842ff,
        0xf4cc4cff, 0x6c5414ff, 0xf4cc54ff, 0x7d682eff,    0x3c2c04ff, 0xcb9b29ff, 0xd2a735ff, 0xc8a54bff,
        0x9c6c04ff, 0x845c04ff, 0x8c640cff, 0x5b440eff,    0x48350cff, 0xba933aff, 0x946404ff, 0xba7e0aff,
        0x9c6c0cff, 0x342404ff, 0x845c0cff, 0xc68b14ff,    0xb9861aff, 0x3c2c0cff, 0xb88727ff, 0xa98438ff,
        0x8c5c04ff, 0xffffffff, 0x94640cff, 0x7b540cff,    0xa97617ff, 0x845c14ff, 0x9c701dff, 0xa87a27ff,
        0x493715ff, 0x957334ff, 0x9c6404ff, 0x845404ff,    0xa56c0bff, 0x9c640cff, 0x8c5c0cff, 0x68450cff,
        0xb67a17ff, 0x9a6614ff, 0x563a0cff, 0x7c5514ff,    0x92661fff, 0x825c1fff, 0x5a492cff, 0x945c0bff,
        0x84540cff, 0x774b0cff, 0xa56a16ff, 0x8c5c14ff,    0x34240cff, 0x1a140bff, 0x3c2404ff, 0x845414ff,
        0x744c14ff, 0x120c04ff, 0x734f1eff, 0x75644dff,    0x8c540cff, 0x643c0cff, 0x955b14ff, 0x452b0bff,
        0x6b4415ff, 0x8c5c1fff, 0x483926ff, 0x844c0cff,    0x8c5414ff, 0x7c4c14ff, 0x84541eff, 0x977a5bff,
        0x8a755eff, 0x744414ff, 0x8c541cff, 0x3c240cff,    0x7d4d1dff, 0x26190cff, 0xa98967ff, 0x997f65ff,
        0x695a4bff, 0x464544ff, 0x844c17ff, 0x593615ff,    0x816c59ff, 0x7c4414ff, 0x673b15ff, 0x7e441cff,
        0x341c0cff, 0x74411cff, 0x8e6041ff, 0x745c4cff,    0x5c3c28ff, 0x904820ff, 0x996547ff, 0x7e4e36ff,
        0xff2b28ff, 0xc7000eff, 0x88000dff, 0x68000aff,    0x0c48ccff, 0x0428a0ff, 0x001474ff, 0x00044cff,
        0x0dca0dff, 0x2da703ff, 0x216c03ff, 0x082800ff,    0xb300b3ff, 0x910091ff, 0x730073ff, 0x400040ff,
        0xf88c14ff, 0xc86010ff, 0x983c10ff, 0x6c200cff,    0x28283cff, 0x1c1c2cff, 0x141420ff, 0x0c0c14ff,
        0x44c1fcff, 0x0680c1ff, 0x045b84ff, 0x044564ff,    0xfcfc48ff, 0xc7aa00ff, 0x887100ff, 0x685600ff,
        0x6f4834ff, 0x572b17ff, 0x492717ff, 0x3c241aff,    0x653e30ff, 0xa66850ff, 0x361c14ff, 0x2a1915ff,
        0x552c24ff, 0x9c5d52ff, 0x92584eff, 0x805049ff,    0x4a2825ff, 0xa45d56ff, 0x362c2bff, 0xb4918fff,
        0xd6aeacff, 0x9d8280ff, 0x574948ff, 0x583b3aff,    0x8c7473ff, 0x240c0cff, 0x0c0404ff, 0x461b1aff,
        0x180c0cff, 0x806868ff, 0x463939ff, 0x685858ff,    0x706363ff, 0x383838ff, 0x282828ff, 0xffffffff,
    };

    // The palette used in-the Hire Troops menu.
    // The first colour (index 0x00) is used for transparent areas.
    const uint32_t PALETTE_HIRE_TROOPS[PALETTE_SIZE] = {
        0x00000000, 0xccb78fff, 0xa4a494ff, 0x8c846cff,    0x9c845cff, 0x9c7c54ff, 0x94744cff, 0x8c7454ff,
        0x846c54ff, 0x7b6747ff, 0x74644cff, 0x6c6454ff,    0xeacf09ff, 0xf0a705ff, 0xfe7f31ff, 0xfe5027ff,
        0xd10404ff, 0x9d1a1aff, 0x645c4cff, 0x6c5c44ff,    0x64543cff, 0x5c543cff, 0x545444ff, 0x4c5444ff,
        0x4c4c3cff, 0x544c3cff, 0x544c34ff, 0x5c4c34ff,    0x644c2cff, 0x64542cff, 0x6c5434ff, 0x745c34ff,
        0x7c542cff, 0x84542cff, 0x8c5c2cff, 0x8c5424ff,    0x9c6434ff, 0xa46c3cff, 0xb4743cff, 0xbc742cff,
        0xc47c34ff, 0xbc844cff, 0xbc8c54ff, 0xb48454ff,    0xac7c4cff, 0xcc8c4cff, 0xe88f37ff, 0xf49c54ff,
        0xcc7414ff, 0xdc6c04ff, 0xbc640cff, 0xac5c0cff,    0xac6c2cff, 0x8c6c44ff, 0x846444ff, 0x7c5c44ff,
        0x6c4c2cff, 0x644424ff, 0x5c442cff, 0x54442cff,    0x4c3c2cff, 0x443c2cff, 0x3c3c2cff, 0x3c3c34ff,
        0x343c34ff, 0x2c3434ff, 0x34342cff, 0x3d2e2eff,    0x3c2c24ff, 0x3c3424ff, 0x443424ff, 0x4c341cff,
        0x5c3c24ff, 0x643c1cff, 0x5c5a20ff, 0x444424ff,    0x444434ff, 0x24342cff, 0x242c2cff, 0x2c2424ff,
        0x2c2c1cff, 0x34241cff, 0x1c1c1cff, 0x14140cff,    0x0c0c0cff, 0x060605ff, 0x707522ff, 0x849324ff,
        0x94ac24ff, 0xa7c921ff, 0xb4dc24ff, 0xd4fc2cff,    0x041404ff, 0x0c1c04ff, 0x101104ff, 0x142c04ff,
        0x1c3404ff, 0x143414ff, 0x143c14ff, 0x144414ff,    0x144c14ff, 0x145414ff, 0x145c14ff, 0x0c4c0cff,
        0x0c440cff, 0x0c3c0cff, 0x04540cff, 0x2c4c0cff,    0x2c440cff, 0x344c0cff, 0x34540cff, 0x44640cff,
        0x4c740cff, 0x547c14ff, 0x548414ff, 0x5c9414ff,    0x649c14ff, 0x6cb41cff, 0x7cd41cff, 0x7c8444ff,
        0x6c6c3cff, 0x8c0000ff, 0x5b2b10ff, 0x201911ff,    0x2c1c14ff, 0x361f07ff, 0x3f2910ff, 0x463415ff,
        0xfbee9aff, 0xeecc81ff, 0xd3c7a5ff, 0xcfcab4ff,    0xc9baa3ff, 0xb7b099ff, 0xaca48cff, 0x9a9897ff,
        0x88949bff, 0x8c8c8cff, 0x7d848aff, 0x76716dff,    0x747c65ff, 0x766a57ff, 0x81693aff, 0x946c2cff,
        0x978872ff, 0x9f947aff, 0xbe9861ff, 0xeab456ff,    0xfc641cff, 0x666b72ff, 0xa4a4a4ff, 0xb4b4b4ff,
        0xb9bdc1ff, 0xccd0cfff, 0xe4d4c4ff, 0xc0321bff,    0x7d4e22ff, 0x605434ff, 0x595856ff, 0x2c1c2cff,
        0xff4155ff, 0xc7000eff, 0x88000dff, 0x68000aff,    0x500008ff, 0x380006ff, 0x44c1fcff, 0x0680c1ff,
        0x045b84ff, 0x044564ff, 0x04354cff, 0x042534ff,    0x41ff96ff, 0x00c760ff, 0x00883eff, 0x006830ff,
        0x005024ff, 0x003819ff, 0xffdd41ff, 0xc7aa00ff,    0x887100ff, 0x685600ff, 0x504200ff, 0x382e00ff,
        0xff8841ff, 0xc75100ff, 0x883400ff, 0x682800ff,    0x501e00ff, 0x381500ff, 0xff41c1ff, 0xc7007fff,
        0x88005aff, 0x680045ff, 0x500036ff, 0x380026ff,    0x4241ffff, 0x0700c7ff, 0x010088ff, 0x010068ff,
        0x000050ff, 0x000038ff, 0xa2a2a2ff, 0x616161ff,    0x3e3e3eff, 0x2b2b2bff, 0x1c1c1cff, 0x0b0b0bff,
        0xffffffff, 0xb1b1b1ff, 0x808080ff, 0x626262ff,    0x484848ff, 0x252525ff, 0x843c0cff, 0xd46414ff,
        0xfc9424ff, 0xfca424ff, 0xfcc434ff, 0xffe432ff,    0xfcfc3cff, 0xfcf46cff, 0xfcfc9cff, 0xe4c44cff,
        0x0202a0ff, 0x050583ff, 0x06066eff, 0x0202c4ff,    0x115585ff, 0x10516dff, 0x1c80dcff, 0x196c97ff,
        0x57350cff, 0x3e280dff, 0x31230cff, 0x1770acff,    0x2476d1ff, 0x255698ff, 0x134572ff, 0x040c3cff,
        0x0c1c64ff, 0x2c3cacff, 0x0c4cccff, 0x3c4cecff,    0x4c5ce4ff, 0x5c6cd4ff, 0x844cc4ff, 0x5414f4ff,
        0x1c84e4ff, 0x3474a4ff, 0x1c741cff, 0x1c9c1cff,    0x34d434ff, 0x44fc44ff, 0xfca4acff, 0xffffffff,
    };
/* clang-format on */

///////////////////////////////////////////////////////////////////////////////
#ifdef WIN32

#include <windows.h>

    /**
     * Attempts to create the given directory.
     */
    bool create_directory(const char* filename) {
        return CreateDirectoryA(filename, NULL)
                || ERROR_ALREADY_EXISTS == GetLastError();
    }

#endif  ////////////////////////////////////////////////////////////////////////

    /**
     * Reads a file to memory.
     */

    void* read_file(std::wstring filename, uint32_t* size) {

        FILE* fp = _wfopen(filename.c_str(), L"rb");
        if (!fp) {
            return NULL;
        }

        fseek(fp, 0L, SEEK_END);
        *size = (uint32_t) ftell(fp);
        fseek(fp, 0L, SEEK_SET);

        void* data = malloc(*size);
        if (!data) {
            fclose(fp);
            return NULL;
        }

        fread(data, *size, 1, fp);

        if (ferror(fp)) {
            free(data);
            data = NULL;
        }
        fclose(fp);

        return data;
    }

    /**
     * Writes an image to disk.
     */
    int write_image(
            std::vector<uint8_t> image,
            const char* filename,
            int w,
            int h,
            const uint32_t* palette) {

        // Open file for writing
        FILE* fp = fopen(filename, "wb");
        if (!fp) {
            perror(filename);
            return 0;
        }

        /*
         * Write using TGA format:
         * http://tfc.duke.free.fr/coding/tga_specs.pdf
         */

        fputc(0x00, fp);  // ID Length
        fputc(0x01, fp);  // Color map type
        fputc(0x01, fp);  // Image type (uncompressed, colour-mapped)

        // Color map specification
        fputc(0, fp);  // Index of first entry
        fputc(0x00, fp);
        fputc(0x00, fp);  // Number of entries (256)
        fputc(0x01, fp);
        fputc(32, fp);  // Entry size (32-bit RGBA)

        // Image specification
        fputc(0x00, fp);  // X-origin
        fputc(0x00, fp);
        fputc(0x00, fp);  // Y-origin
        fputc(0x00, fp);
        fputc((uint8_t) w, fp);  // Width
        fputc((uint8_t)(w >> 8), fp);
        fputc((uint8_t) h, fp);  // Height
        fputc((uint8_t)(h >> 8), fp);
        fputc(8, fp);  // Bits per pixel

        // Image descriptor byte
        // (8 = number of alpha bits, bit5: upper-left origin)
        fputc(8 | 1 << 5, fp);

        // Colour map data
        for (int i = 0; i < PALETTE_SIZE; ++i) {

            const uint32_t col = palette[i];
            const uint8_t red = (uint8_t)((col & 0xFF000000) >> 24);
            const uint8_t green = (uint8_t)((col & 0x00FF0000) >> 16);
            const uint8_t blue = (uint8_t)((col & 0x0000FF00) >> 8);
            const uint8_t alpha = (uint8_t)(col & 0x000000FF);

            fputc(blue, fp);
            fputc(green, fp);
            fputc(red, fp);
            fputc(alpha, fp);
        }

        // Pixel data
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const uint8_t index = image[x + y * w];
                fputc(index, fp);
            }
        }

        fclose(fp);
        return 1;
    }

    std::uint16_t readShort(const uint8_t* data, int& offset) {
        // little endian
        std::uint16_t value = std::uint16_t(
                data[offset + 1] << 8
                | data[offset + 0]);
        offset += 2;
        return value;
    }

    std::uint8_t readByte(const uint8_t* data, int& offset) {
        std::uint8_t value = std::uint8_t(data[offset]);
        offset += 1;
        return value;
    }

    void extractImage(
            const std::string& outputDir,
            const uint8_t* data,
            int& offset,
            const uint32_t* palette) {

        int initialOffset = offset;

        // Read dimensions
        std::uint16_t w = readShort(data, offset);
        std::uint16_t h = readShort(data, offset);
        int imageSize = w * h;
        std::vector<uint8_t> pixels(imageSize);

        // Skip 4 unknown bytes
        offset += 4;

        // Read pixels
        int nextPxIndex = 0;
        while (nextPxIndex < imageSize) {

            int next = readByte(data, offset);

            if (next > 0x80) {
                // Next 'n' pixels are all the same
                int numSamePixels = next - 0x80;
                int paletteIndex = readByte(data, offset);
                for (int i = 0; i < numSamePixels; i++) {
                    pixels[nextPxIndex] = paletteIndex;
                    nextPxIndex++;
                }

            } else {
                // Next 'n' pixels are all different
                int numDifferentPixels = next;
                for (int i = 0; i < numDifferentPixels; i++) {
                    int paletteIndex = readByte(data, offset);
                    pixels[nextPxIndex] = paletteIndex;
                    nextPxIndex++;
                }
            }
        }

        // Save the rendered image to disk
        // TODO: These don't get sorted properly by other applications since hex
        //   characters don't play nicely with alphabetical order (e.g.
        //   img_00A46D78 precedes img_00002131).
        //   N.B. Sorting by created date does the trick!
        char filename[256];
        snprintf(filename,
                sizeof(filename),
                "%s\\img_ui_%08X.tga",
                outputDir.c_str(), initialOffset);
        write_image(pixels, filename, w, h, palette);

        // Skip some unknown bytes
        // TODO: Trying to read these "skipped" bytes causes the program to crash
        //   after the last image, which leads me to believe these bytes are
        //   probably supposed to come BEFORE each image, not after it
        std::uint16_t next = readShort(data, offset);
        if (next != 0x03) {
            offset += 2;
        }
    }

    void extractImages(const uint8_t* data, const std::string& outputDir) {

        int offset = OFFSET_TITLE;

        // Title image
        extractImage(outputDir, data, offset, PALETTE_TITLE);

        // Menu backgrounds
        for (int i = 0; i < 9; i++) {
            extractImage(outputDir, data, offset, PALETTE_MENU);
        }

        // "Hire Troops" background
        extractImage(outputDir, data, offset, PALETTE_HIRE_TROOPS);

        // Campaign backgrounds
        for (int i = 0; i < 3; i++) {
            extractImage(outputDir, data, offset, PALETTE_MENU);
        }

        // Loading screen backgrounds
        for (int i = 0; i < 2; i++) {
            extractImage(outputDir, data, offset, PALETTE_LOADING);
        }

        // Campaign level intros and menu components
        for (int i = 0; i < 128; i++) {
            extractImage(outputDir, data, offset, PALETTE_MENU);
        }

        // "Hire Troops" menu components
        for (int i = 0; i < 20; i++) {
            extractImage(outputDir, data, offset, PALETTE_HIRE_TROOPS);
        }

        // Menu components continued
        // TODO: A few of these might still be using the wrong palette, but it
        //   doesn't really matter too much for our purposes since we choose the
        //   palette when we render
        for (int i = 0; i < 65; i++) {
            extractImage(outputDir, data, offset, PALETTE_MENU);
        }

        // Loading screen components
        for (int i = 0; i < 14; i++) {
            extractImage(outputDir, data, offset, PALETTE_LOADING);
        }

        // "Hire Troops" menu icons
        offset = OFFSET_HIRE_TROOP_ICONS;
        for (int i = 0; i < 239; i++) {
            extractImage(outputDir, data, offset, PALETTE_HIRE_TROOPS);
        }

        // Game icons
        for (int i = 0; i < 592; i++) {
            extractImage(outputDir, data, offset, PALETTE_GAME);
        }
    }

    void extractUiImages(std::wstring inputFile, std::string outputDir) {

        // Read the file
        uint32_t size;
        uint8_t* data = (uint8_t*) read_file(inputFile, &size);
        if (!data) {
            throw std::runtime_error("Error reading Interfac.dat\n");
        }

        extractImages(data, outputDir);

        free(data);
    }

}  // namespace InterfaceExtractor
