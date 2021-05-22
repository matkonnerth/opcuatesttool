import { Component, OnInit } from '@angular/core';
import { JobsService } from '../jobs.service';
import { Script } from '../script';
import { ScriptService } from '../script.service';
import { Request } from '../job';
import { Target} from '../target';
import { TargetService } from '../target.service';

@Component({
  selector: 'app-script',
  templateUrl: './script.component.html',
  styleUrls: ['./script.component.css']
})
export class ScriptComponent implements OnInit {

  scripts: Script[] = [];
  targets: Target[] = [];
  selectedScript?: Script;
  content?: string;
  target: Target = { name: 'noTarget', host: 'localhost', port: 9888};

  constructor(private scriptService: ScriptService, private jobService: JobsService, private targetService: TargetService) { }

  ngOnInit(): void {
    this.getScripts();
    this.getTargets();
  }

  onSelect(script: Script): void {
    this.selectedScript = script;
    this.scriptService.getScriptContent(script).subscribe(c => this.content = c);
  }

  getScripts(): void {
    this.scriptService.getScripts()
      .subscribe(s => this.scripts = s);
  }

  getTargets(): void {
    this.targetService.getTargets()
      .subscribe(t => this.targets = t);
  }

  newJob(script: Script): void {

    const req: Request = {
      name: 'myJob',
      script: script.name,
      serverUri: 'opc.tcp://' + this.target.host + ':' + this.target.port
    };
    this.jobService.createJob(req);
  }

  updateScript(script: Script, content: string): void
  {
    this.scriptService.updateScript(script, content);
  }

  onChange(e: Target): void {
    console.log(e);
    this.target = e;
  }


}
