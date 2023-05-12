/*
 * File:   CODE.c
 * Author: BTL
 *
 * Created on May 12, 2023, 10:40 AM
 */

// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <ds1307.c> 


#define LCD_ENABLE_PIN  PIN_D1
#define LCD_RS_PIN      PIN_D2
#define LCD_DATA4       PIN_D0
#define LCD_DATA5       PIN_C2
#define LCD_DATA6       PIN_C1
#define LCD_DATA7       PIN_C0
#include <lcd.h>

#define tang RB2
#define giam RB3
#define mode RB1 
#define dat_bao RB0
#define coi  RD3
#define rl   RA1
#define led  RA0 

uint8_t ngay, thang, nam, gio, phut, giay;
uint8_t chinh = 0;
uint8_t ct = 0;

//----------------------------------------
int8_t gio_on[10], phut_on[10];
int8_t giay_off[10];
int8_t lan_hen;
int8_t chon_lan;
int8_t giay_nho;
int8_t giay_tat;
//----------------------------------------
bool bct = false;
bool np = true;

void hien_thi();
void quet_phim();
void luu_rom();
void doc_rom();
void bao_gio();
//----------------------------------------

    
void main()
{
  
    
    lcd_init();
    ds1307_init();
    doc_rom();
    chon_lan=1;
    output_low(coi);
    giay_tat=0;
    output_high(led);
    

    while(true)
    {
        hien_thi();
        
        if (chinh == 0)
        {
            ds1307_get_date(&ngay, &thang, &nam);
            ds1307_get_time(&gio, &phut, &giay);
            if(giay_nho!=giay)
            {
                bao_gio();
                giay_nho=giay;
                if(giay_tat > 0)
                {
                    giay_tat--;
                    if(giay_tat == 0)
                    {
                        output_low(coi);
                        output_low(rl);
                    }
                }
            }
        }
        else if (chinh > 3)
        {
            ds1307_get_time(&gio, &phut, &giay);
        }
        
        int8_t i;
        for (i = 0; i < 200; i++)
        {
            quet_phim();
            __delay_us(300);
        }
    }
}
//----------------------------------------
void bao_gio()
{
    int8_t i;
    if(giay_tat == 0 && giay == 0)
    {
        for(i = 0; i < lan_hen; i++)
        {
            if(gio == gio_on[i] && phut == phut_on[i] && (gio_on[i] != 0 || phut_on[i] != 0))
            {
                giay_tat = giay_off[i];
                output_high(coi);
                output_high(rl);
            }
        }
    }
}

//----------------------------------------
void doc_rom()
{
    int8_t i;
    uint8_t dl;
    doc_byte_ds1307(10, &dl);
    if (dl != 0x55)
    {
        lan_hen = 2;
        for (i = 0; i < 10; i++)
        {
            gio_on[i] = 0;
            phut_on[i] = 0;
            giay_off[i] = 1;
        }
        luu_rom();
    }
    else
    {
        doc_byte_ds1307(11, &lan_hen);
        for (i = 13; i < 23; i++)
        {
            doc_byte_ds1307(i, &dl);
            gio_on[i - 13] = dl;
        }
        for (i = 23; i < 33; i++)
        {
            doc_byte_ds1307(i, &dl);
            phut_on[i - 23] = dl;
        }
        for (i = 33; i < 43; i++)
        {
            doc_byte_ds1307(i, &dl);
            giay_off[i - 33] = dl;
        }
    }
}
//----------------------------------------
void luu_rom()
{
    ghi_byte_ds1307(10, 0x55);
    ghi_byte_ds1307(11, lan_hen);
    int8_t i;
    for (i = 13; i < 23; i++)
    {
        ghi_byte_ds1307(i, gio_on[i - 13]);
    }
    for (i = 23; i < 33; i++)
    {
        ghi_byte_ds1307(i, phut_on[i - 23]);
    }
    for (i = 33; i < 43; i++)
    {
        ghi_byte_ds1307(i, giay_off[i - 33]);
    }
}
//----------------------------------------
void hien_thi() 
{
   if (chinh!=0) 
   {
      ct++;
      if (ct==4)
      {
         ct=1;  
         bct=!bct; 
      }
   }
   else
   {
      ct=0;
      bct=0; 
   }
//----------------------------------------
   if(chinh<7) 
   {
      lcd_gotoxy(1,1); 
      printf(lcd_putc, "TIME:  "); 
      if(!(bct&&(chinh==1)))
      {
         printf (lcd_putc,"%02u",gio); 
      }
      else
      {
         printf (lcd_putc,"  "); 
      }
      printf (lcd_putc,":");
   
      if(!(bct&&(chinh==2)))
      {
         printf (lcd_putc,"%02u",phut);
      }
      else
      {
         printf (lcd_putc,"  ");
      }
      printf (lcd_putc,":");
      
      if(!(bct&&(chinh==3)))
      {
         printf (lcd_putc,"%02u   ",giay);
      }
      else
      {
         printf (lcd_putc,"     ");
      }
   //---------------------------------------- 
      lcd_gotoxy (1, 2);
      printf (lcd_putc, "DATE: ");
      if(!(bct&&(chinh==4)))
      {
         printf (lcd_putc,"%02u",ngay);
      }
      else
      {
         printf (lcd_putc,"  ");
      }
      lcd_putc("-");
      
      if(!(bct&&(chinh==5)))
      {
         printf (lcd_putc,"%02u",thang);
      }
      else
      {
         printf (lcd_putc,"  ");
      }
      lcd_putc("-");
      
      if(!(bct&&(chinh==6)))
      {
         printf (lcd_putc,"20%02u",nam);
      }
      else
      {
         printf (lcd_putc,"20  ");
      }
   }  
   else 
   {
      if(chinh==7)  
      {
         if(!(bct&&(chinh==7)))
         {
            lcd_gotoxy (1, 1);
            printf (lcd_putc, "TONG SO LAN BAO ");
            lcd_gotoxy (1, 2);
            printf (lcd_putc,"      %02u         ",lan_hen);
         }
         else
         {
            lcd_gotoxy (1, 2);
            printf (lcd_putc,"                ");
         }
      }
      else
      {
         if(!(bct&&(chinh==8))) 
         {
            lcd_gotoxy (1, 1);
            printf (lcd_putc, "THOI GIAN LAN:%02u",chon_lan);
         }
         else
         {
            lcd_gotoxy (1, 1);
            printf (lcd_putc,"THOI GIAN LAN:  ");
         }
                 
         lcd_gotoxy (1, 2);
         printf (lcd_putc, "ON:");
         if(!(bct&&(chinh==9))) 
         {
            printf (lcd_putc,"%02u",gio_on[chon_lan-1]);
         }
         else
         {
            printf (lcd_putc,"  ");
         }
         printf (lcd_putc,":");
      
         if(!(bct&&(chinh==10)))
         {
            printf (lcd_putc,"%02u",phut_on[chon_lan-1]);
         }
         else
         {
            printf (lcd_putc,"  ");
         }
         printf (lcd_putc," OFF:"); 
         
         if(!(bct&&(chinh==11)))
         {
            printf (lcd_putc,"%02us   ",giay_off[chon_lan-1]);
         }
         else
         {
            printf (lcd_putc,"  ");
         }
      }
   }
}
//----------------------------------------
void quet_phim(){if (np) 
{
    if (input(tang)==0)
    {
        ct=0;
        bct=0;
        np=0;
        switch (chinh)
        {
            case 1:
            {
                gio++;
                if (gio==24)
                    gio=0;
                break;
            }
            case 2:
            {
                phut++;
                if (phut==60)
                    phut=0;
                break;
            }
            case 3:
            {
                giay++;
                if (giay==60)
                    giay=0;
                break;
            }
       
            case 4:
            {
                ngay++;
                if (ngay==32)
                    ngay=1;
                break;
            }
            case 5:
            {
                thang++;
                if (thang==13)
                    thang=1;
                break;
            }
            case 6:
            {
                nam++;
                if (nam==100)
                    nam=0;
                break;
            }
            case 7:
            {
                lan_hen++;
                if (lan_hen==11)
                    lan_hen=1;
                chon_lan=1;
                break;
            }
            
            case 8:
            {
                chon_lan++;
                if (chon_lan>lan_hen)
                    chon_lan=1;
                break;
            }
            case 9:
            {
                gio_on[chon_lan-1]++;
                if (gio_on[chon_lan-1]>23)
                    gio_on[chon_lan-1]=0;
                break;
            }
            case 10:
            {
                phut_on[chon_lan-1]++;
                if (phut_on[chon_lan-1]==60)
                    phut_on[chon_lan-1]=0;
                break;
            }
            case 11:
            {
                giay_off[chon_lan-1]++;
                if (giay_off[chon_lan-1]==200)
                    giay_off[chon_lan-1]=1;
                break;
            }
        }
    }
}
if (input(giam)==0)
{
    ct=0;
    bct=0;
    np=0;
    switch (chinh)
    {
        case 1:
        {
            gio--;
            if (gio==255)
                gio=23;
            break;
        }
        case 2:
        {
            phut--;
            if (phut==255)
                phut=59;
            break;
        }
        case 3:
        {
            giay--;
            if (giay==255)
                giay=59;
            break;
        }
        case 4:
        {
            ngay--;
            if (ngay==0)
                ngay=31;
            break;
        }
        case 5:
        {
            thang--;
            if (thang==0)
                thang=12;
            break;
        }
        case 6:
        {
            nam--;
            if (nam==255)
                nam=99;
            break;
        }
        case 7:
        {
            lan_hen--;
            if (lan_hen==0)
                lan_hen=10;
            chon_lan=1;
            break;
        }
        case 8:
        {
            chon_lan--;
            if (chon_lan==0)
                chon_lan=lan_hen;
            break;
        }
        case 9:
        {
            gio_on[chon_lan-1]--;
            if (gio_on[chon_lan-1]>23)
                gio_on[chon_lan-1]=23;
            break;
        }
        case 10:
        {
            phut_on[chon_lan-1]--;
            if (phut_on[chon_lan-1]>59)
                phut_on[chon_lan-1]=59;
            break;
        }
        case 11:
        {
            giay_off[chon_lan-1]--;
            if (giay_off[chon_lan-1]==0)
                giay_off[chon_lan-1]=199;
            break;
        }
    }
}
if (input(mode)==0)
{
    ct=0;
    bct=0;
    np=0; 
    if(chinh<7) 
    {
        chinh++;
        if (chinh==4)
        {
            ds1307_dat_gpg(gio,phut,giay);
        }
        if (chinh==7)
        {
            chinh=0;
            ds1307_dat_tntn(ngay,thang,nam);
        }
    }
    
    if(chinh>=7) 
    {
        chinh++;
        if (chinh==12)
        {
            chinh=8;
        }
    }
}
//---------------------------------------- 
if (input(dat_bao)==0)
{
    ct=0;
    bct=0;
    np=0; 
    if (chinh<7)
    {
        chinh=7; 
    }
    else
    {
        chinh=0;
        luu_rom();
    }
}
else
{
    if(input(tang) && input(giam) && input(mode) && input(dat_bao))
        np=1; 
}

}




