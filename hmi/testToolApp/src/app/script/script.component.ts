import { Component, OnInit } from '@angular/core';
import { Script } from '../script';
import { ScriptService } from '../script.service';

@Component({
  selector: 'app-script',
  templateUrl: './script.component.html',
  styleUrls: ['./script.component.css']
})
export class ScriptComponent implements OnInit {

  scripts: Script[] = [];
  selectedScript?: Script;
  content?: string;

  constructor(private scriptService: ScriptService) { }

  ngOnInit(): void {
    this.getScripts();
  }

  onSelect(script: Script): void {
    this.selectedScript = script;
    this.scriptService.getScriptContent(script).subscribe(c => this.content = c);
  }

  getScripts(): void {
    this.scriptService.getScripts()
      .subscribe(s => this.scripts = s);
  }

}
